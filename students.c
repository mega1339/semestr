#include "students.h"

struct students *students_create(struct students *pool) {
  // Allocating a buffer for structure or use existing if pool was provided as argument
  struct students *buffer = pool == NULL ? malloc(sizeof(struct students)) : pool;
  // Construct a dynamic array
  dynamic_array_create(&buffer->arr, sizeof(struct student), students_item_constructor, students_item_destructor, NULL);
  // Do not release if we didn't allocate by ourselves
  buffer->self_created = pool == buffer;
  return buffer;
}

void students_destroy(struct students *pool) {
  if (pool == NULL)
    return;
  dynamic_array_destroy(&pool->arr);
  // Do not release if we didn't allocate by ourselves
  if (!pool->self_created)
    return;
  free(pool);
}

struct student *students_insert(struct students *pool, student_insert_data data) {
  // Find for item in structure
  struct student *found = students_find_by_uid(pool, data.record_book_uid);
  if (found)
    return found;
  // Insert if we didn't find it and return a pointer
  return (struct student *)dynamic_array_insert(&pool->arr, &data, NULL);
}

bool students_remove(struct students *pool, struct student *at) {
  if (at == NULL)
    return false;
  return dynamic_array_remove(&pool->arr, (void *)at);
}

bool students_remove_by_uid(struct students *pool, const char *uid) {
  // Find for item in structure by uid
  struct student *item = students_find_by_uid(pool, uid);
  if (item == NULL)
    return false;
  // Remove if found
  return dynamic_array_remove(&pool->arr, (void *)item);
}

struct student *students_find_by_uid(struct students *pool, const char *uid) {
  // If there are some students
  if (students_size(pool) > 0) {
    for (struct student *item = students_first(pool); item <= students_last(pool); ++item) {
      // If student pointer is not null and record books equals
      if (item != NULL && strcmp(item->record_book_uid, uid) == 0) {
        return item;
      }
    }
  }
  return NULL;
}

struct student *students_find_by_surname(struct students *pool, const char *surname) {
  // If there are some students
  for (struct student *item = students_first(pool); item <= students_last(pool); ++item) {
    // If student pointer is not null and surnames equals
    if (item != NULL && strcmp(item->surname, surname) == 0) {
      return item;
    }
  }
  return NULL;
}

struct student *students_first(struct students *students) {
  // Return pointer to first item
  return (struct student *)dynamic_array_first(&students->arr);
}

struct student *students_last(struct students *pool) {
  // Return pointer to last item
  return (struct student *)dynamic_array_last(&pool->arr);
}

struct student *students_end(struct students *pool) {
  // Return pointer to the end of buffer
  return (struct student *)dynamic_array_end(&pool->arr);
}

size_t students_size(struct students *pool) {
  // Count of students
  return dynamic_array_size(&pool->arr);
}

void students_item_constructor(struct dynamic_array *arr, void *item, void *data) {
  // Students constructor, gets clalled by dynamic_array_insert

  student_insert_data *insert_data = (student_insert_data *)data;
  struct student *position = (struct student *)item;

  // Copy each of values because the provided data will locate to temporary buffer
  position->record_book_uid = copy_string(insert_data->record_book_uid);
  position->surname = copy_string(insert_data->surname);
  position->name = copy_string(insert_data->name);
  position->patronymic = copy_string(insert_data->patronymic);
  position->faculty = copy_string(insert_data->faculty);
  position->speciality = copy_string(insert_data->speciality);
}

void students_item_destructor(struct dynamic_array *arr, void *item) {
  struct student *this_item = (struct student *)item;
  // Safe release of each item in structure
  SAFE_FREE(this_item->surname);
  SAFE_FREE(this_item->name);
  SAFE_FREE(this_item->patronymic);
  SAFE_FREE(this_item->faculty);
  SAFE_FREE(this_item->speciality);
}

struct students *students_create_from_csv(struct students *pool, struct csv_data *csv) {
  struct students *buffer = students_create(pool);
  for (struct csv_row *row = csv_data_first(csv); row <= csv_data_last(csv); ++row) {
    if (row == NULL || csv_row_size(row) < 6)
      continue;
    student_insert_data data;
    // Get each of values, but do not copy values because the data is temporary
    data.record_book_uid = *csv_row_get_at(row, 0);
    data.surname = *csv_row_get_at(row, 1);
    data.name = *csv_row_get_at(row, 2);
    data.patronymic = *csv_row_get_at(row, 3);
    data.faculty = *csv_row_get_at(row, 4);
    data.speciality = *csv_row_get_at(row, 5);
    // Insert item
    students_insert(buffer, data);
  }
  return buffer;
}

void students_save_csv_to_file(struct students *pool, FILE *fp) {
  static char buffer[512 + 1];
  for (struct student *entry = students_first(pool); entry <= students_last(pool); ++entry) {
    if (entry == NULL)
      continue;
    memset(buffer, 0, sizeof(buffer));
    sprintf_s(buffer,
              sizeof(buffer),
              "%s;%s;%s;%s;%s;%s\n",
              entry->record_book_uid,
              entry->surname,
              entry->name,
              entry->patronymic,
              entry->faculty,
              entry->speciality);
    fwrite(buffer, sizeof(char), strlen(buffer), fp);
  }
}
