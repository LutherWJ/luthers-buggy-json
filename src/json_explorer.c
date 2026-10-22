#include "json_explorer.h"
#include "json_parser.h"
#include <string.h>

JsonNode_t *json_findkey(JsonNode_t *json, char *target) {
  if (json == NULL || json->type != JSON_OBJECT) {
    return NULL;
  }

  for (size_t i = 0; i < json->json_object.keys.count; i++) {
    char *key = array_get(&json->json_object.keys, i);
    if (strcmp(target, key) == 0) {
      return (JsonNode_t *)array_get(&json->json_object.values, i);
    }
  }

  return NULL; // Key was not found
}

JsonNode_t *json_findkey_recursive(JsonNode_t *json, char *target) {
  if (json == NULL || json->type != JSON_OBJECT) {
    return NULL;
  }

  ArrayList obj_list;
  array_create(&obj_list, sizeof(JsonNode_t *), ARRAYLIST_INIT_SIZE);

  for (size_t i = 0; i < json->json_object.keys.count; i++) {
    char *key = array_get(&json->json_object.keys, i);
    JsonNode_t *value = array_get(&json->json_object.values, i);
    if (strcmp(target, key) == 0) {
      array_free(&obj_list);
      return value;
    }

    if (value->type == JSON_OBJECT) {
      array_push(&obj_list, value);
    }
  }

  for (size_t i = 0; i < obj_list.count; i++) {
    JsonNode_t **nested_obj = array_get(&obj_list, i);
    JsonNode_t *found_obj = json_findkey_recursive(*nested_obj, target);
    if (found_obj != NULL) {
      array_free(&obj_list);
      return found_obj;
    }
  }

  array_free(&obj_list);
  return NULL; // Key was not found
}
