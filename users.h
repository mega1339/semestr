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

struct user {
  const char *name;
  const char *password;
  bool can_view_edit_books;
  bool can_view_edit_students;
};

typedef struct user user_insert_data;

struct users {
  struct dynamic_array arr;
  bool self_created;
};

struct users *users_create(struct users *pool);
void users_destroy(struct users *pool);
struct user *users_insert(struct users *pool, user_insert_data data);
bool users_remove(struct users *pool, struct user *at);
bool users_remove_by_name(struct users *pool, const char *name);

struct user *users_find_by_name(struct users *pool, const char *name);

struct user *users_first(struct users *pool);
struct user *users_last(struct users *pool);
struct user *users_end(struct users *pool);

size_t users_size(struct users *pool);

// Helpers
void users_item_constructor(struct dynamic_array *arr, void *item, void *data);
void users_item_destructor(struct dynamic_array *arr, void *item);

struct users *users_create_from_csv(struct users *pool, struct csv_data *csv);
void users_save_csv_to_file(struct users *pool, FILE *fp);
