#include <stddef.h>

#ifndef ARRAY_LIST
#define ARRAY_LIST

typedef struct {
  void *data;
  size_t item_size;
  size_t count;
  size_t capacity;
} ArrayList;

int array_create(ArrayList *list, size_t item_size, size_t initial_capacity);
int array_push(ArrayList *list, const void *item);
void *array_get(const ArrayList *list, size_t index);
void array_free(ArrayList *list);

#endif
