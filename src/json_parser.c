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
    free_lexer(&lexer);
    return token_result;
  }

  // Print output to make sure nothing is bugged (it is)
  for (size_t i = 0; i < lexer.token_list.count; i++) {
    Token_t *token = array_get(&lexer.token_list, i);
    switch (token->token_type) {
    case LBRACE_TOK:
      printf("Opening object\n");
      break;
    case RBRACE_TOK:
      printf("Close object\n");
      break;
    case LBRACKET_TOK:
      printf("Open array\n");
      break;
    case RBRACKET_TOK:
      printf("Close array\n");
      break;
    case COLON_TOK:
      printf("Colon (between key and value)\n");
      break;
    case COMMA_TOK:
      printf("Comma delimiter\n");
      break;
    case STR_TOK: {
      // print string content within quotes
      size_t length = token->value.str_slice.length + 1;
      char str[length];
      snprintf(str, length, "%s", lexer.text);
      printf("%s\n", str);
      break;
    }
    case NUM_TOK:
      // print out the number
      printf("number: %f\n", token->value.number_value);
      break;
    default:
      printf("Misc Token\n");
    }
  }

  free_lexer(&lexer);
  return 0;
}

int build_tree(const ArrayList *tokens, JsonNode_t *json_tree) { return 0; }
