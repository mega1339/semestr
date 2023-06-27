#include "books.h"

struct books *books_create(struct books *pool) {
  // Allocating a buffer for structure or use existing if pool was provided as argument
  struct books *buffer = pool == NULL ? malloc(sizeof(struct books)) : pool;
  // Construct a dynamic array
  dynamic_array_create(&buffer->arr, sizeof(struct book), books_item_constructor, books_item_destructor, NULL);
  // Do not release if we didn't allocate by ourselves
  buffer->self_created = pool == buffer;
  return buffer;
}

void books_destroy(struct books *pool) {
  if (pool == NULL)
    return;
  dynamic_array_destroy(&pool->arr);
  // Do not release if we didn't allocate by ourselves
  if (!pool->self_created)
    return;
  free(pool);
}

struct book *books_insert(struct books *pool, book_insert_data data) {
  // Find for item in structure
  struct book *found = books_find_by_uid(pool, data.uid);
  if (found != NULL)
    return found;
  // Insert if we didn't find it and return a pointer
  return (struct book *)dynamic_array_insert(&pool->arr, &data, NULL);
}

bool books_remove(struct books *pool, struct book *at) {
  return dynamic_array_remove(&pool->arr, (void *)at);
}

bool books_remove_by_uid(struct books *pool, uint64_t uid) {
  // Find for item in structure
  struct book *item = books_find_by_uid(pool, uid);
  if (item == NULL)
    return false;
  // Remove if found
  return dynamic_array_remove(&pool->arr, (void *)item);
}

struct book *books_find_by_uid(struct books *pool, uint64_t uid) {
  // If there are some items
  if (books_size(pool) > 0) {
    for (struct book *item = books_first(pool); item <= books_last(pool); ++item) {
      // If pointer is not null and uid equals
      if (item != NULL && item->uid == uid) {
        return item;
      }
    }
  }
  return NULL;
}

struct book *books_first(struct books *books) {
  // Return pointer to first item
  return (struct book *)dynamic_array_first(&books->arr);
}

struct book *books_last(struct books *pool) {
  // Return pointer to last item
  return (struct book *)dynamic_array_last(&pool->arr);
}

struct book *books_end(struct books *pool) {
  // Return pointer to the end of buffer
  return (struct book *)dynamic_array_end(&pool->arr);
}

size_t books_size(struct books *pool) {
  // Count of books
  return dynamic_array_size(&pool->arr);
}

void books_item_constructor(struct dynamic_array *arr, void *item, void *data) {
  // Books constructor, gets clalled by dynamic_array_insert

  book_insert_data *insert_data = (book_insert_data *)data;
  struct book *buffer_first = (struct book *)dynamic_array_first(arr);
  struct book *buffer_last = (struct book *)dynamic_array_last(arr);
  struct book *position = buffer_last + 1 /*get reserved space, not the last one*/;

  // If there are some items
  if (dynamic_array_size(arr) > 0) {
    // If the first item uid more than inserting item uid
    if (buffer_first->uid >= insert_data->uid) {
      // Insert the item at the first position
      position = buffer_first;
      // Move all items for 1 item down
      dynamic_array_move(arr, (void *)(position + 1), (void *)position);
    } else if (buffer_last->uid <= insert_data->uid) {
      /* Nothing to do */
    } else {
      for (struct book *current_position = position - 1 /*skip reserved*/; current_position >= buffer_first;
           --current_position) {
        // If the inserting item uid more than iterating item uid
        if (insert_data->uid >= current_position->uid) {
          // Put the item down than current item
          position = current_position + 1;
          // Move all items for 1 item down
          dynamic_array_move(arr, (void *)(position + 1), (void *)position);
          break;
        }
      }
    }
  } else {
    // Nothing is there, so it can be created at the start of buffer
    position = buffer_first;
  }

  position->uid = insert_data->uid;
  position->authors = copy_string(insert_data->authors);
  position->book_name = copy_string(insert_data->book_name);
  position->available_amount = insert_data->available_amount;
  position->total_amount = insert_data->total_amount;
}

void books_item_destructor(struct dynamic_array *arr, void *item) {
  // Books destructor, gets called by dynamic_array_remove

  struct book *this_item = (struct book *)item;
  // Safe release of each item in structure
  SAFE_FREE(this_item->authors);
  SAFE_FREE(this_item->book_name);
}

struct books *books_create_from_csv(struct books *pool, struct csv_data *csv) {
  struct books *buffer = books_create(pool);
  for (struct csv_row *row = csv_data_first(csv); row <= csv_data_last(csv); ++row) {
    if (csv_row_size(row) < 5)
      continue;
    book_insert_data data;
    // Get each of values, but do not copy values because the data is temporary
    data.uid = strtoull(*csv_row_get_at(row, 0), NULL, 10);
    data.authors = *csv_row_get_at(row, 1);
    data.book_name = *csv_row_get_at(row, 2);
    data.total_amount = strtoul(*csv_row_get_at(row, 3), NULL, 10);
    data.available_amount = strtoul(*csv_row_get_at(row, 4), NULL, 10);
    // Insert item
    books_insert(buffer, data);
  }
  return buffer;
}

void books_save_csv_to_file(struct books *pool, FILE *fp) {
  static char buffer[512 + 1];
  for (struct book *entry = books_first(pool); entry <= books_last(pool); ++entry) {
    if (entry == NULL)
      continue;
    memset(buffer, 0, sizeof(buffer));
    sprintf_s(buffer,
              sizeof(buffer),
              "%llu;%s;%s;%d;%d\n",
              entry->uid,
              entry->authors,
              entry->book_name,
              (int)entry->available_amount,
              (int)entry->total_amount);
    fwrite(buffer, sizeof(char), strlen(buffer), fp);
  }
}
