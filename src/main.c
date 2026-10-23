#include "array_list.h"
#include "json_explorer.h"
#include "json_parser.h"
#include <stdio.h>
#include <stdlib.h>

#define INITIAL_BUFFER_SIZE 128

int main(void) {
  const char filename[10] = "test.json";

  // Get file handle
  FILE *fptr = fopen(filename, "r");
  if (fptr == NULL) {
    printf("Failed to open file");
    return 1;
  }

  // Allocate memory to hold the text content of the file
  ArrayList file_buf;
  int result = array_create(&file_buf, sizeof(char), INITIAL_BUFFER_SIZE);
  if (result != 0) {
    return 1;
  }

  // Read file 4 bytes at a time
  int c;
  while ((c = fgetc(fptr)) != EOF) {
    char byte = (char)c;
    result = array_push(&file_buf, &byte);
    if (result != 0) {
      return 1;
    }
  }

  // Strings in C must be terminated with \0
  char sentinal = '\0';
  result = array_push(&file_buf, &sentinal);
  if (result != 0) {
    return 1;
  }
  JsonNode_t *json = parse_json((char *)file_buf.data);

  // Time to verify if it actually worked by searching for a specific value
  // This code will be tailored specifically for the test.json file

  JsonNode_t *node = array_get(&json->json_array, 0); // Grab first object
  JsonNode_t *order_id = json_findkey(node, "OrderID");
  printf("Attempted to locate OrderID of first object in test.json\n");
  if (order_id != NULL && order_id->type == JSON_STRING) {
    printf("OrderID: %s\n", order_id->string_value);
  } else {
    printf("Search failed\n");
  }

  free_json(json);
  array_free(&file_buf);
  fclose(fptr);
  return 0;
}
