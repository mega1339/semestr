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

struct student {
  const char *record_book_uid;
  const char *surname;
  const char *name;
  const char *patronymic;
  const char *faculty;
  const char *speciality;
};

typedef struct student student_insert_data;

struct students {
  struct dynamic_array arr;
  bool self_created;
};

struct students *students_create(struct students *pool);
void students_destroy(struct students *pool);
struct student *students_insert(struct students *pool, student_insert_data data);
bool students_remove(struct students *pool, struct student *at);
bool students_remove_by_uid(struct students *pool, const char *uid);

struct student *students_find_by_uid(struct students *pool, const char *uid);
struct student *students_find_by_surname(struct students *pool, const char *surname);

struct student *students_first(struct students *pool);
struct student *students_last(struct students *pool);
struct student *students_end(struct students *pool);

size_t students_size(struct students *pool);

// Helpers
void students_item_constructor(struct dynamic_array *arr, void *item, void *data);
void students_item_destructor(struct dynamic_array *arr, void *item);

struct students *students_create_from_csv(struct students *pool, struct csv_data *csv);
void students_save_csv_to_file(struct students *pool, FILE *fp);
