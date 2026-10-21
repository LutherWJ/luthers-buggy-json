#ifndef JSON_LEXER
#define JSON_LEXER

#include "array_list.h"
#include <stdbool.h>
#include <stddef.h>

#define TOKEN_LIST_LENGTH 16

typedef enum {
  RBRACE_TOK,
  LBRACE_TOK,
  RBRACKET_TOK,
  LBRACKET_TOK,
  COMMA_TOK,
  COLON_TOK,
  STR_TOK,
  NUM_TOK,
  BOOL_TOK,
  NULL_TOK,
  INVALID_TOK
} TokenType_t;

typedef struct {
  TokenType_t token_type;
  union {
    struct {
      size_t start;  // Start position of the string
      size_t length; // Length of the string
    } str_slice;
    bool bool_value;
    double number_value;
  } value;
} Token_t;

typedef struct {
  char *text;
  size_t text_length;
  size_t pos;
  ArrayList token_list;
} Lexer_t;

int tokenize_json(Lexer_t *lexer);
int init_lexer(Lexer_t *lexer, char *text, size_t text_length);
void free_lexer(Lexer_t *lexer);
void skip_whitespace(Lexer_t *lexer);
Token_t parse_string(Lexer_t *lexer);
Token_t parse_number(Lexer_t *lexer);
Token_t parse_keyword(Lexer_t *lexer);
Token_t parse_token(Lexer_t *lexer);

#endif
