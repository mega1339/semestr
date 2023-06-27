#pragma once

#include <stdint.h>
#include <stdio.h>
#include <stdbool.h>
#include <stdlib.h>
#include <string.h>

#include "dynamic_array.h"
#include "csv/csv_parser.h"
#include "csv/csv_row.h"
#include "common.h"

struct book {
  uint64_t uid;
  const char *authors;
  const char *book_name;
  size_t available_amount;
  size_t total_amount;
};

typedef struct book book_insert_data;

struct books {
  struct dynamic_array arr;
  bool self_created;
};

struct books *books_create(struct books *pool);
void books_destroy(struct books *pool);
struct book *books_insert(struct books *pool, book_insert_data data);
bool books_remove(struct books *pool, struct book *at);
bool books_remove_by_uid(struct books *pool, uint64_t uid);

struct book *books_find_by_uid(struct books *pool, uint64_t uid);

struct book *books_first(struct books *pool);
struct book *books_last(struct books *pool);
struct book *books_end(struct books *pool);

size_t books_size(struct books *pool);

// Helpers
void books_item_constructor(struct dynamic_array *arr, void *item, void *data);
void books_item_destructor(struct dynamic_array *arr, void *item);

struct books *books_create_from_csv(struct books *pool, struct csv_data *csv);
void books_save_csv_to_file(struct books *pool, FILE *fp);
