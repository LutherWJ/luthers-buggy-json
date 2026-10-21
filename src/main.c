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
  char *file_buf = malloc(sizeof(char) * INITIAL_BUFFER_SIZE);
  size_t length = 0;
  size_t capacity = INITIAL_BUFFER_SIZE;

  // Read file 4 bytes at a time
  int c;
  while ((c = fgetc(fptr)) != EOF) {

    // Double the size of our text buffer if we run out of space
    if (length >= capacity - 1) {
      capacity *= 2;
      char *new_buf = realloc(file_buf, capacity);
      file_buf = new_buf;

      // Always make sure the allocation was successful
      if (file_buf == NULL) {
        printf("Failed to allocate memory");
        free(file_buf);
        fclose(fptr);
        return 1;
      }
    }

    file_buf[length++] = (char)c;
  }

  // Strings in C must be terminated with \0
  file_buf[length] = '\0';

  // Allocate our memory for our JSON data structure
  JsonNode_t *out_ptr = malloc(sizeof(JsonNode_t));
  if (out_ptr == NULL) {
    printf("Failed to allocate memory");
    free(file_buf);
    fclose(fptr);
    return 1;
  }

  JsonNode_t *json = parse_json(file_buf);

  printf("done\n");
  free_json(json);
  free(file_buf);
  fclose(fptr);
  return 0;
}
