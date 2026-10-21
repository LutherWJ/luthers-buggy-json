#include "array_list.h"
#include "json_lexer.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef JSON_PARSER
#define JSON_PARSER

#define ARRAYLIST_INIT_SIZE 4

typedef enum {
  JSON_NULL,
  JSON_STRING,
  JSON_NUMBER,
  JSON_BOOL,
  JSON_OBJECT,
  JSON_ARRAY,
} JsonType_t;

typedef struct JsonNode_t JsonNode_t;

struct JsonNode_t {
  JsonType_t type;
  union {
    char *string_value;
    double number_value;
    bool boolean_value;
    ArrayList json_array;
    struct {
      ArrayList keys;
      ArrayList values;
    } json_object;
  };
};

typedef struct {
  size_t pos;
  const ArrayList *token_list;
  JsonNode_t *root;
} TreeBuilder_t;

JsonNode_t *parse_json(char *json_text);
JsonNode_t *build_tree(const ArrayList *tokens);
int build_object(TreeBuilder_t *builder, JsonNode_t *out_ptr);
int build_array(TreeBuilder_t *builder, JsonNode_t *out_ptr);
int build_value(TreeBuilder_t *builder, JsonNode_t *out_ptr);
int build_pair(TreeBuilder_t *builder, JsonNode_t *out_ptr);
Token_t *get_next_token(TreeBuilder_t *builder);
void print_token(char *json_text, Token_t *token);
void free_json(JsonNode_t *root);

#endif
