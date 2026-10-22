#include "json_parser.h"

#ifndef JSON_EXPLORER
#define JSON_EXPLORER

JsonNode_t *json_findkey(JsonNode_t *json, char *key);
JsonNode_t *json_findkey_recursive(JsonNode_t *json, char *key);

#endif
