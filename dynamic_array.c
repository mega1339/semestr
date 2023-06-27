#include "dynamic_array.h"

struct dynamic_array *dynamic_array_create(struct dynamic_array *at, size_t item_size,
                                           dynamic_array_item_constructor constructor,
                                           dynamic_array_item_destructor destructor,
                                           dynamic_array_item_finder finder) {
  // Allocating a buffer for structure or use existing if pool was provided as the first argument
  struct dynamic_array *arr = at == NULL ? malloc(sizeof(*arr)) : at;
  arr->buffer = NULL;
  arr->size = 0;
  arr->capacity = 0;
  arr->item_size = item_size;
  arr->item_constructor = constructor;
  arr->item_destructor = destructor;
  arr->item_finder = finder;
  arr->self_created = arr != at;
  return arr;
}

void dynamic_array_destroy(struct dynamic_array *arr) {
  if (arr == NULL)
    return;

  uintptr_t buffer_first = dynamic_array_first_intptr(arr);
  uintptr_t buffer_last = dynamic_array_last_intptr(arr);

  if (arr->buffer != NULL) {
    for (uintptr_t item = buffer_last; item >= buffer_first; item -= arr->item_size) {
      if (item != 0) {
        dynamic_array_remove(arr, (void *)item);
      }
    }
    free(arr->buffer);
  }

  if (arr->self_created)
    free(arr);
}

void dynamic_array_reserve(struct dynamic_array *arr, size_t amount) {
  if (arr->capacity >= amount)
    return; // The buffer capacity is already enough for this value

  arr->capacity = amount;
  size_t new_buffer_size = amount * arr->item_size;
  void *new_buffer = malloc(new_buffer_size); // Allocate a buffer
  memset(new_buffer, 0, new_buffer_size); // Zeroify the whole buffer
  if (arr->buffer != NULL) {
    // Move the values of previous buffer to new buffer
    memmove_s(new_buffer, new_buffer_size, arr->buffer, arr->size * arr->item_size);
    // Release the previous buffer
    free(arr->buffer);
  }
  arr->buffer = new_buffer;
}

void *dynamic_array_insert(struct dynamic_array *arr, void *data, void *at) {
  if (arr->item_constructor == NULL)
    return NULL; // Constructor was provided as the dynamic array create argument before

  if (at == NULL) {
    dynamic_array_reserve(arr, arr->size + 1);
    at = (void *)(dynamic_array_last_intptr(arr) + arr->item_size); // after the last item
  }

  void *buffer_first = dynamic_array_first(arr);
  void *buffer_end = dynamic_array_end(arr);

  if (!(at >= buffer_first && at <= buffer_end))
    return NULL; // Do not create if position is out of bounds

  arr->item_constructor(arr, at, data); // Call item constructor
  ++arr->size;
  return at;
}

bool dynamic_array_remove(struct dynamic_array *arr, void *at) {
  void *buffer_first = dynamic_array_first(arr);
  void *buffer_last = dynamic_array_last(arr);

  if (!(at >= buffer_first && at <= buffer_last))
    return false; // Do not remove if position is out of bounds

  if (arr->item_destructor != NULL)
    arr->item_destructor(arr, at); // Call destructor if was provided
  dynamic_array_move(arr, at, (void *)((uintptr_t)at + arr->item_size)); // Move items up
  --arr->size;
  return true;
}

bool dynamic_array_move(struct dynamic_array *arr, void *dest, void *src) {
  uintptr_t buffer_first = dynamic_array_first_intptr(arr);
  uintptr_t buffer_last = dynamic_array_last_intptr(arr);
  uintptr_t buffer_end = dynamic_array_end_intptr(arr);
  uintptr_t dest_int = (uintptr_t)dest;
  uintptr_t src_int = (uintptr_t)src;

  size_t src_size = buffer_last + arr->item_size - src_int;
  size_t dest_capacity = buffer_end - dest_int;

  if (!(dest_int >= buffer_first && dest_int <= buffer_end))
    return false; // Return false if destination is out of bounds
  if (!(src_int >= buffer_first && src_int <= buffer_last))
    return false; // Return false if source is out of bounds
  if (src_size <= 0)
    return true; // Nothing to copy, but it's ok

  memmove_s(dest,
            dest_capacity,
            src,
            src_size);

  return true;
}

void *dynamic_array_find(struct dynamic_array *arr, void *data) {
  if (arr->item_finder == NULL)
    return NULL;
  return arr->item_finder(arr, data);
}

void *dynamic_array_get_at(struct dynamic_array *arr, size_t position) {
  if (!(position >= 0 && position <= arr->size - 1))
    return NULL; // Return null if position is out of bounds
  return (void *)(dynamic_array_first_intptr(arr) + (position * arr->item_size));
}

void *dynamic_array_first(struct dynamic_array *arr) {
  // Start of buffer
  return arr->buffer;
}

void *dynamic_array_last(struct dynamic_array *arr) {
  // The last item of buffer
  return (void *)(dynamic_array_first_intptr(arr) + ((arr->size - 1) * arr->item_size));
}

void *dynamic_array_end(struct dynamic_array *arr) {
  // The end of buffer
  return (void *)(dynamic_array_first_intptr(arr) + (arr->capacity * arr->item_size));
}

uintptr_t dynamic_array_first_intptr(struct dynamic_array *arr) {
  // Start of buffer as integer
  return (uintptr_t)dynamic_array_first(arr);
}

uintptr_t dynamic_array_last_intptr(struct dynamic_array *arr) {
  // The last item of buffer as integer
  return (uintptr_t)dynamic_array_last(arr);
}

uintptr_t dynamic_array_end_intptr(struct dynamic_array *arr) {
  // The end of buffer as integer
  return (uintptr_t)dynamic_array_end(arr);
}

size_t dynamic_array_size(struct dynamic_array *arr) {
  // Amount of items in buffer
  return arr->size;
}

size_t dynamic_array_capacity(struct dynamic_array *arr) {
  // The reserved capacity of buffer
  return arr->capacity;
}
