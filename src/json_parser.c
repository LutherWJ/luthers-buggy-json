#include "json_parser.h"
#include "json_lexer.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

JsonNode_t *parse_json(char *json_text) {
  // Initialize our lexer
  Lexer_t lexer;
  init_lexer(&lexer, json_text, strlen(json_text));

  // Convert text into "tokens"
  int token_result = tokenize_json(&lexer);
  if (token_result != 0) {
    free_lexer(&lexer);
    return NULL;
  }

  // Print output to make sure nothing is bugged (it is)
  printf("=================TOKEN OUTPUT===================\n");
  for (size_t i = 0; i < lexer.token_list.count; i++) {
    Token_t *token = array_get(&lexer.token_list, i);
    print_token(lexer.text, token);
  }
  printf("===============END TOKEN OUTPUT=================\n");

  // Begin building the data structure in memory
  JsonNode_t *json_tree = build_tree(&lexer.token_list, lexer.text);

  // clean up memory
  free_lexer(&lexer);
  return json_tree;
}

JsonNode_t *build_tree(const ArrayList *tokens, char *json_text) {
  if (tokens == NULL) {
    return NULL;
  }

  JsonNode_t *json_tree = malloc(sizeof(JsonNode_t));
  if (json_tree == NULL) {
    return NULL;
  }

  // Start at position 1 since we immediately read the first token
  Token_t *start_token = (Token_t *)array_get(tokens, 0);
  TreeBuilder_t builder = {1, tokens, json_tree, json_text};

  // JSON either starts with an array or an object, nothing else.
  switch (start_token->token_type) {
  case LBRACE_TOK: {

    int result = build_object(&builder, json_tree);
    if (result != 0) {
      free_json(json_tree);
      return NULL;
    }
    break;
  }
  case LBRACKET_TOK: {
    int result = build_array(&builder, json_tree);
    if (result != 0) {
      free_json(json_tree);
      return NULL;
    }
    break;
  }
  default:
    printf("Invalid starting token");
    return NULL;
  }

  return json_tree;
}

int build_object(TreeBuilder_t *builder, JsonNode_t *out_ptr) {
  // INITIALIZTION!!!
  out_ptr->type = JSON_OBJECT;
  array_create(&out_ptr->json_object.keys, sizeof(char *), ARRAYLIST_INIT_SIZE);
  array_create(&out_ptr->json_object.values, sizeof(JsonNode_t),
               ARRAYLIST_INIT_SIZE);

  while (builder->pos < builder->token_list->count) {

    // Get the next token and make sure it exists
    Token_t *next_tok = array_get(builder->token_list, builder->pos);
    builder->pos++;
    if (next_tok == NULL) {
      return 1;
    }

    switch (next_tok->token_type) {
    case STR_TOK:;
      // string here means we have a key value pair to parse!
      // Allocate the key
      char *key = malloc(sizeof(char) * next_tok->value.str_slice.length + 1);
      if (key == NULL) {
        return 1;
      }

      // slice the string into the buffer
      size_t key_start = next_tok->value.str_slice.start * sizeof(char);
      snprintf(key, next_tok->value.str_slice.length + 1, "%s",
               builder->json_text + key_start);
      key[next_tok->value.str_slice.length] = '\0';

      // push the pointer to our key list
      int result = array_push(&out_ptr->json_object.keys, key);
      if (result != 0) {
        free(key);
        return 1;
      }

      // Next token after a key should be a colon
      next_tok = get_next_token(builder);
      if (next_tok->token_type != COLON_TOK) {
        // No colon here means its garbag json, pack it up.
        return 1;
      }

      // Extract the value
      JsonNode_t value;
      result = build_value(builder, &value);
      if (result != 0) {
        return 1;
      }

      // Push it to the value list
      result = array_push(&out_ptr->json_object.values, &value);
      if (result != 0) {
        return 1;
      }

      break;
    case RBRACE_TOK:
      // Object is closed jobs done.
      return 0;
    default:
      // invalid token placement = invalid json
      printf("[ERROR] Invalid token placement\n");
      return 1;
    }

    next_tok = get_next_token(builder);
    switch (next_tok->token_type) {
    case RBRACE_TOK:
      // Object is terminated
      return 0;
    case COMMA_TOK:
      // Expect more values
      // note that real json does not allow trailing commas but I DO!!!
      continue;
    default:
      return 1; // Invalid token!!!!!!!
    }
  }
  printf("[ERROR] Object was not terminated\n");
  return 1; // Object wasn't terminated, invalid JSON
}

int build_array(TreeBuilder_t *builder, JsonNode_t *out_ptr) {
  // Initialize the array
  out_ptr->type = JSON_ARRAY;
  int result = array_create(&out_ptr->json_array, sizeof(JsonNode_t),
                            ARRAYLIST_INIT_SIZE);
  if (result != 0) {
    printf("[ERROR] Failed to initialize array");
    return 1;
  }

  while (builder->pos < builder->token_list->count) {
    Token_t *token = array_get(builder->token_list, builder->pos);

    // Check for array termination
    if (token->token_type == RBRACKET_TOK) {
      return 0;
    }

    // get the value
    JsonNode_t value;
    result = build_value(builder, &value);
    if (result != 0) {
      return 1;
    }

    // Push it to the array!
    result = array_push(&out_ptr->json_array, &value);
    if (result != 0) {
      return 1;
    }

    token = get_next_token(builder);
    // The only valid tokens after a value are comma and closing bracket
    switch (token->token_type) {
    case RBRACKET_TOK:
      return 0;
    case COMMA_TOK:
      continue;
    default:
      return 1;
    }
  }
  return 1; // unterminated array
}

// Only handles string allocations,
// expects the node to be allocated and pushed to the proper place by the
// caller (Does however handle the allocation of strings)
int build_value(TreeBuilder_t *builder, JsonNode_t *out_ptr) {
  if (out_ptr == NULL) {
    return 1;
  }
  Token_t *token = array_get(builder->token_list, builder->pos);
  builder->pos++;

  switch (token->token_type) {
  case STR_TOK: {
    size_t length = token->value.str_slice.length + 1;
    // Allocate string buffer
    char *value = malloc(sizeof(char) * length);
    if (value == NULL) {
      return 1;
    }

    // Slice value
    size_t value_start = token->value.str_slice.start * sizeof(char);
    snprintf(value, length, "%s", builder->json_text + value_start);
    value[length] = '\0';

    // populate node
    out_ptr->type = JSON_STRING;
    out_ptr->string_value = value;
    break;
  }
  case LBRACE_TOK: {
    return build_object(builder, out_ptr);
  }
  case LBRACKET_TOK: {
    return build_array(builder, out_ptr);
  }
  case NUM_TOK:
    out_ptr->type = JSON_NUMBER;
    out_ptr->number_value = token->value.number_value;
    break;
  case BOOL_TOK:
    out_ptr->type = JSON_BOOL;
    out_ptr->boolean_value = token->value.bool_value;
    break;
  case NULL_TOK:
    out_ptr->type = JSON_NULL;
    break;
  default:
    printf("[ERROR] Invalid token where value should be\n");
    return 1; // Invalid value
  }

  return 0;
}

// Recursively frees all nested nodes
void free_json(JsonNode_t *root) {
  if (root == NULL) {
    return;
  }

  switch (root->type) {
  case JSON_OBJECT: {
    // Free all of the string buffers allocated for the keys
    for (size_t i = 0; i < root->json_object.keys.count; i++) {
      char *key = array_get(&root->json_object.keys, i);
      if (key != NULL) {
        free(key);
      }
    }

    // recursivley call this function for nested JSON nodes
    for (size_t i = 0; i < root->json_object.values.count; i++) {
      JsonNode_t *value = array_get(&root->json_object.values, i);
      if (value->type == JSON_STRING) {
        free(value->string_value);
      } else if (value->type == JSON_OBJECT || value->type == JSON_ARRAY) {
        free_json(value);
      }
    }

    array_free(&root->json_object.keys);
    array_free(&root->json_object.values);
    break;
  }
  case JSON_ARRAY: {
    for (size_t i = 0; i < root->json_array.count; i++) {
      JsonNode_t *value = array_get(&root->json_array, i);
      if (value->type == JSON_STRING) {
        free(value->string_value);
      }
    }
    array_free(&root->json_array);
    break;
  }
  case JSON_STRING: {

    if (root->string_value != NULL) {
      free(root->string_value);
    }
    break;
  }
    free(root);
  }
}

void print_token(char *json_text, Token_t *token) {
  switch (token->token_type) {
  case LBRACE_TOK:
    printf("\nOpening object\n");
    break;
  case RBRACE_TOK:
    printf("\nClose object\n");
    break;
  case LBRACKET_TOK:
    printf("Open array\n");
    break;
  case RBRACKET_TOK:
    printf("Close array\n");
    break;
  case COLON_TOK:
    printf(": ");
    break;
  case COMMA_TOK:
    printf(",\n");
    break;
  case STR_TOK: {
    // print string content within quotes
    size_t length = token->value.str_slice.length + 1;
    // Pointer arithmatic to find the address of the start of our string val
    size_t start_offset = token->value.str_slice.start * sizeof(char);
    char str[length];

    // Null terminate the string
    str[length] = '\0';

    // slice and print
    snprintf(str, length, "%s", json_text + start_offset);
    printf("\"%s\"", str);
    break;
  }
  case NUM_TOK:
    // print out the number
    printf("%f", token->value.number_value);
    break;
  default:
    printf("Misc Token\n");
  }
}

// stupid helper for code dedup
inline Token_t *get_next_token(TreeBuilder_t *builder) {
  if (builder->pos < builder->token_list->count) {
    Token_t *token = array_get(builder->token_list, builder->pos);
    builder->pos++;
    return token;
  }
  return NULL;
}
