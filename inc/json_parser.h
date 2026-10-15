#include "array_list.h"
#include <stdbool.h>
#include <stddef.h>

#ifndef JSON_PARSER
#define JSON_PARSER

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

    struct {
      char *keys;
      struct JsonNode_t **values;
      size_t count;
      size_t capacity;

    } json_object;

    struct {
      struct JsonNode_t **values;
      size_t count;
      size_t capacity;
    } json_array;
  };
};

int parse_json(char *json_text, JsonNode_t *out_ptr);
int build_tree(const ArrayList *tokens, JsonNode_t *json_tree);

#endif
