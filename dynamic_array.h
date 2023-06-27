#pragma once

#include <memory.h>
#include <stdlib.h>
#include <stdbool.h>

#include "common.h"

struct dynamic_array;

typedef void (*dynamic_array_item_constructor)(struct dynamic_array *arr, void *item, void *data);
typedef void (*dynamic_array_item_destructor)(struct dynamic_array *arr, void *item);
typedef void **(*dynamic_array_item_finder)(struct dynamic_array *arr, void *data);

struct dynamic_array {
  void *buffer;
  size_t size;
  size_t capacity;
  size_t item_size;
  dynamic_array_item_constructor item_constructor;
  dynamic_array_item_destructor item_destructor;
  dynamic_array_item_finder item_finder;
  bool self_created;
};

struct dynamic_array *dynamic_array_create(struct dynamic_array *at,
                                           size_t item_size,
                                           dynamic_array_item_constructor constructor,
                                           dynamic_array_item_destructor destructor,
                                           dynamic_array_item_finder finder);
void dynamic_array_destroy(struct dynamic_array *arr);

void dynamic_array_reserve(struct dynamic_array *arr, size_t amount);

void *dynamic_array_insert(struct dynamic_array *arr, void *data, void *at);
bool dynamic_array_remove(struct dynamic_array *arr, void *at);
bool dynamic_array_move(struct dynamic_array *arr, void *dest, void *src);

void *dynamic_array_find(struct dynamic_array *arr, void *data);
void *dynamic_array_get_at(struct dynamic_array *arr, size_t position);

void *dynamic_array_first(struct dynamic_array *arr);
void *dynamic_array_last(struct dynamic_array *arr);
void *dynamic_array_end(struct dynamic_array *arr);

uintptr_t dynamic_array_first_intptr(struct dynamic_array *arr);
uintptr_t dynamic_array_last_intptr(struct dynamic_array *arr);
uintptr_t dynamic_array_end_intptr(struct dynamic_array *arr);

size_t dynamic_array_size(struct dynamic_array *arr);
size_t dynamic_array_capacity(struct dynamic_array *arr);
