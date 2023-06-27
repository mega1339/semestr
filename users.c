#include "users.h"

struct users *users_create(struct users *pool) {
  // Allocating a buffer for structure or use existing if pool was provided as argument
  struct users *buffer = pool == NULL ? malloc(sizeof(struct users)) : pool;
  // Construct a dynamic array
  dynamic_array_create(&buffer->arr, sizeof(struct user), users_item_constructor, users_item_destructor, NULL);
  // Do not release if we didn't allocate by ourselves
  buffer->self_created = pool == buffer;
  return buffer;
}

void users_destroy(struct users *pool) {
  if (pool == NULL)
    return;
  dynamic_array_destroy(&pool->arr);
  // Do not release if we didn't allocate by ourselves
  if (!pool->self_created)
    return;
  free(pool);
}

struct user *users_insert(struct users *pool, user_insert_data data) {
  // Find for item in structure
  struct user *found = users_find_by_name(pool, data.name);
  if (found)
    return found;
  // Insert if we didn't find it and return a pointer
  return (struct user *)dynamic_array_insert(&pool->arr, &data, NULL);
}

bool users_remove(struct users *pool, struct user *at) {
  return dynamic_array_remove(&pool->arr, (void *)at);
}

bool users_remove_by_name(struct users *pool, const char *name) {
  // Find for item in structure by name
  struct user *item = users_find_by_name(pool, name);
  if (item == NULL)
    return false;
  // Remove if found
  return dynamic_array_remove(&pool->arr, (void *)item);
}

struct user *users_find_by_name(struct users *pool, const char *name) {
  // If there are some items
  if (users_size(pool) > 0) {
    for (struct user *item = users_first(pool); item <= users_last(pool); ++item) {
      // If pointer is not null and names equals
      if (item != NULL && strcmp(item->name, name) == 0) {
        return item;
      }
    }
  }
  return NULL;
}

struct user *users_first(struct users *users) {
  // Return pointer to first item
  return (struct user *)dynamic_array_first(&users->arr);
}

struct user *users_last(struct users *pool) {
  // Return pointer to last item
  return (struct user *)dynamic_array_last(&pool->arr);
}

struct user *users_end(struct users *pool) {
  // Return pointer to the end of buffer
  return (struct user *)dynamic_array_end(&pool->arr);
}

size_t users_size(struct users *pool) {
  // Count of users
  return dynamic_array_size(&pool->arr);
}

void users_item_constructor(struct dynamic_array *arr, void *item, void *data) {
  // Users constructor, gets clalled by dynamic_array_insert

  user_insert_data *insert_data = (user_insert_data *)data;
  struct user *position = (struct user *)item;

  // Copy each of values because the provided data will locate to temporary buffer
  position->name = copy_string(insert_data->name);
  position->password = copy_string(insert_data->password);
  position->can_view_edit_students = insert_data->can_view_edit_students;
  position->can_view_edit_books = insert_data->can_view_edit_books;
}

void users_item_destructor(struct dynamic_array *arr, void *item) {
  // Books destructor, gets called by dynamic_array_remove

  struct user *this_item = (struct user *)item;
  // Safe release of each item in structure
  SAFE_FREE(this_item->name);
  SAFE_FREE(this_item->password);
}

struct users *users_create_from_csv(struct users *pool, struct csv_data *csv) {
  struct users *buffer = users_create(pool);
  for (struct csv_row *row = csv_data_first(csv); row <= csv_data_last(csv); ++row) {
    if (row == NULL || csv_row_size(row) < 3)
      continue;
    user_insert_data data;
    // Get each of values, but do not copy values because the data is temporary
    data.name = *csv_row_get_at(row, 0);
    data.password = *csv_row_get_at(row, 1);
    data.can_view_edit_students = **csv_row_get_at(row, 2) == '1';
    data.can_view_edit_books = **csv_row_get_at(row, 3) == '1';
    // Insert item
    users_insert(buffer, data);
  }
  return buffer;
}

void users_save_csv_to_file(struct users *pool, FILE *fp) {
  static char buffer[512 + 1];
  for (struct user *entry = users_first(pool); entry <= users_last(pool); ++entry) {
    if (entry == NULL)
      continue;
    memset(buffer, 0, sizeof(buffer));
    sprintf_s(buffer,
              sizeof(buffer),
              "%s;%s;%d;%d\n",
              entry->name,
              entry->password,
              entry->can_view_edit_students ? 1 : 0,
              entry->can_view_edit_books ? 1 : 0);
    fwrite(buffer, sizeof(char), strlen(buffer), fp);
  }
}
