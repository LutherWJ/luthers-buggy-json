#include "json_parser.h"
#include "json_lexer.h"
#include <stdio.h>
#include <string.h>

int parse_json(char *json_text, JsonNode_t *out_ptr) {
  // Initialize our lexer
  Lexer_t lexer;
  init_lexer(&lexer, json_text, strlen(json_text));

  // Convert text into "tokens"
  int token_result = tokenize_json(&lexer);
  if (token_result < 0) {
    return token_result;
  }

  // Print output to make sure nothing is bugged (it is)
  for (size_t i = 0; i < lexer.token_list->count; i++) {
    Token_t *token = array_get(lexer.token_list, i);
    switch (token->token_type) {
    case STR_TOK: {
      size_t length = token->value.str_slice.length + 1;
      char str[length];
      snprintf(str, length, "%s", lexer.text);
      printf("%s\n", str);
      break;
    }
    case NUM_TOK:
      printf("number: %f\n", token->value.number_value);
      break;
    default:
      printf("Misc Token\n");
    }
  }
  return 0;
}

int build_tree(const ArrayList *tokens, JsonNode_t *json_tree) { return 0; }
