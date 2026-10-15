#include "json_lexer.h"
#include "array_list.h"
#include <ctype.h>
#include <stddef.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

int init_lexer(Lexer_t *lexer, char *text, size_t text_length) {
  ArrayList token_list;
  int list_err = array_create(&token_list, sizeof(Token_t), TOKEN_LIST_LENGTH);
  if (list_err < 0) {
    return -1;
  }

  lexer->token_list = &token_list;
  lexer->text = text;
  lexer->text_length = text_length;
  lexer->pos = 0;

  return 0;
}

Token_t parse_string(Lexer_t *lexer) {
  Token_t token;
  token.token_type = STR_TOK;

  lexer->pos++;
  size_t start_pos = lexer->pos; // Record start index of the string

  token.value.str_slice.start = lexer->text + (sizeof(char) + start_pos);

  // Iterate through until we reach the closing quote
  while (lexer->pos < lexer->text_length && lexer->text[lexer->pos] != '"') {
    lexer->pos++;
  }

  // Record length of string
  token.value.str_slice.length = lexer->pos - start_pos;
  lexer->pos++; // advance past closing quote

  return token;
}

Token_t parse_number(Lexer_t *lexer) {
  Token_t token;

  // Record starting point of number and keep iterating until we reach a non
  // numeric character
  size_t start_pos = lexer->pos;
  while (isdigit(lexer->text[lexer->pos]) && lexer->pos < lexer->text_length) {
    lexer->pos++;
  }

  // Record start and end
  size_t end_pos = lexer->pos;
  size_t length = end_pos - start_pos;
  size_t size = (sizeof(char) * length);

  // slice the string into a character array then convert to a double
  char str[length];
  snprintf(str, size, "%s", lexer->text + start_pos);
  double num = strtod(str, NULL);

  token.token_type = NUM_TOK;
  token.value.number_value = num;
  return token;
}

// Parse keywords such as "true", "false" and "null"
Token_t parse_keyword(Lexer_t *lexer) {
  Token_t token;

  size_t start_pos = lexer->pos;
  // continue reading as long as the characters are alphabetical
  while (isalpha(lexer->text[lexer->pos]) && lexer->pos < lexer->text_length) {
    lexer->pos++;
  }

  // Record the start and end positions of the keyword
  size_t end_pos = lexer->pos;
  size_t length = end_pos - start_pos;
  size_t size = (sizeof(char) * length);

  // Copy the string into our string buffer (C doesn't have a slice function)
  char str[length];
  snprintf(str, size, "%s", lexer->text + start_pos);

  // if statements cannot compare strings in C
  // instead we ue strcmp (short for "string compare")
  if (strcmp(str, "true") == 0) {
    token.token_type = BOOL_TOK;
    token.value.bool_value = true;
  } else if (strcmp(str, "false") == 0) {
    token.token_type = BOOL_TOK;
    token.value.bool_value = false;
  } else if (strcmp(str, "null") == 0) {
    token.token_type = NULL_TOK;
  } else {
    token.token_type = INVALID_TOK;
  }

  return token;
}

Token_t parse_token(Lexer_t *lexer) {
  Token_t token;

  // Read a character and determine the type of token
  char c = lexer->text[lexer->pos];
  if (c == '{') {
    token.token_type = LBRACE_TOK;
    lexer->pos++;
  } else if (c == '}') {
    token.token_type = RBRACE_TOK;
    lexer->pos++;
  } else if (c == '[') {
    token.token_type = LBRACKET_TOK;
    lexer->pos++;
  } else if (c == ']') {
    token.token_type = RBRACKET_TOK;
    lexer->pos++;
  } else if (c == ':') {
    token.token_type = COLON_TOK;
    lexer->pos++;
  } else if (c == ',') {
    token.token_type = COMMA_TOK;
    lexer->pos++;
  } else if (c == '"') {
    token = parse_string(lexer);
  } else if (isdigit(c)) {
    token = parse_number(lexer);
  } else if (isalpha(c)) {
    token = parse_keyword(lexer);
  } else { // some random symbol not in a string means its invalid json
    token.token_type = INVALID_TOK;
  }
  return token;
}

void skip_whitespace(Lexer_t *lexer) {
  while (lexer->pos < lexer->text_length && isspace(lexer->text[lexer->pos])) {
    lexer->pos++;
  }
}

int tokenize_json(Lexer_t *lexer) {
  // Parse text start to finish
  while (lexer->pos < lexer->text_length) {
    // whitespace is meaningless outside of a string, skip it!
    skip_whitespace(lexer);
    Token_t token = parse_token(lexer);

    // invalid token = invalid json. our job here is done.
    if (token.token_type == INVALID_TOK) {
      return -1; // anything other than 0 means there was an error
    }

    // push the token to our token array
    array_push(lexer->token_list, &token);
  }
  return 0;
}
