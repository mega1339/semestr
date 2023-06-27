#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#define WIN32_LEAN_AND_MEAN
#include <windows.h>

#include "books.h"
#include "students.h"
#include "users.h"

#include "csv/csv_parser.h"

size_t get_size_of_file(FILE *fp) {
  if (fp == NULL)
    return 0;

  // Get the current position of file
  long current_position = ftell(fp);

  size_t fsize = 0;
  // Go to the end of file
  fseek(fp, 0L, SEEK_END);
  // Get the size of file
  fsize = ftell(fp);
  // Go back to the previous position of file
  fseek(fp, current_position, SEEK_SET);

  return fsize;
}

const char *read_file_data(FILE *fp) {
  if (fp == NULL)
    return 0;

  // Go to the start of file
  fseek(fp, 0L, SEEK_SET);
  // Get the size of file
  size_t file_size = get_size_of_file(fp);
  // Allocate the file size buffer
  char *buffer = malloc(file_size + 1);
  // Read file data to buffer
  fread(buffer, sizeof(char), file_size, fp);
  // Zero-terminate a buffer
  buffer[file_size] = '\0';

  return (const char *)buffer;
}

struct books *parse_books() {
  FILE *fp = NULL;
  fopen_s(&fp, "./books.csv", "r");
  if (fp == NULL) {
    return NULL;
  }

  struct csv_data *data = NULL;
  const char *buffer = read_file_data(fp);
  fclose(fp);
  data = parse_csv(buffer);
  SAFE_FREE(buffer);

  struct books *pool = books_create_from_csv(NULL, data);
  free(data);
  return pool;
}

struct students *parse_students() {
  FILE *fp = NULL;
  fopen_s(&fp, "./students.csv", "r");
  if (fp == NULL) {
    return NULL;
  }

  size_t fsize = get_size_of_file(fp);

  struct csv_data *data = NULL;
  const char *buffer = read_file_data(fp);
  fclose(fp);
  data = parse_csv(buffer);
  SAFE_FREE(buffer);

  struct students *pool = students_create_from_csv(NULL, data);
  free(data);
  return pool;
}

struct users *parse_users() {
  FILE *fp = NULL;
  fopen_s(&fp, "./users.csv", "r");
  if (fp == NULL) {
    return NULL;
  }

  struct csv_data *data = NULL;
  const char *buffer = read_file_data(fp);
  fclose(fp);
  data = parse_csv(buffer);
  SAFE_FREE(buffer);

  struct users *pool = users_create_from_csv(NULL, data);
  free(data);
  return pool;
}

static struct books *books_pool = NULL;
static struct students *students_pool = NULL;
static struct users *users_pool = NULL;
static struct user *this_user = NULL;

const char *get_user_input(const char *text) {
  static char user_input_buffer[144 + 1];
  printf("%s", text);
  memset((void *)user_input_buffer, 0, sizeof(user_input_buffer));
  fgets(user_input_buffer, sizeof(user_input_buffer), stdin);
  user_input_buffer[strlen(user_input_buffer) - 1] = '\0'; // remove the newline character (\n)
  return user_input_buffer;
}

void pause_and_exit(const char *text, int code) {
  printf("%s", text);
  system("pause");
  exit(code);
}

void ask_for_auth() {
  if (this_user)
    return;

  this_user = users_find_by_name(users_pool, get_user_input("������� ��� ������������: "));
  const char *password = get_user_input("������� ������: ");

  if (this_user == NULL || strcmp(this_user->password, password) != 0) {
    pause_and_exit("������������ �� ������, ���� ������������ ������. ��������� ��������� ������.\n", 0);
  }
}

void difficulty_1_books_add() {
  uint64_t uid = strtoull(get_user_input("������� ����� ISBN: "), NULL, 10);
  if (uid == 0 || books_find_by_uid(books_pool, uid) != NULL) {
    printf("������������ ����� ISBN, ���� ����� ����� ��� ����������.\n");
    difficulty_1_books_add();
    return;
  }

  book_insert_data data;
  data.uid = uid;
  // Copy all values because user input buffer is temporary, gets changed after every call of get_user_input
  data.book_name = copy_string(get_user_input("������� �������� �����: "));
  data.authors = copy_string(get_user_input("������� ������� �����: "));
  data.available_amount = strtoul(get_user_input("������� ���������� ��������� ����: "), NULL, 10);
  data.total_amount = strtoul(get_user_input("������� ���������� ���� �����: "), NULL, 10);

  books_insert(books_pool, data);

  SAFE_FREE(data.book_name);
  SAFE_FREE(data.authors);

  printf("����� ���������.\n");
}

void difficulty_1_books_remove() {
  uint64_t uid = strtoull(get_user_input("������� ����� ISBN: "), NULL, 10);
  if (uid == 0 || books_remove_by_uid(books_pool, uid) == false) {
    printf("������������ ����� ISBN, ���� ����� ����� �� ����������.\n");
    difficulty_1_books_remove();
    return;
  }

  printf("����� �������.\n");
}

void difficulty_1_books_view_by_uid() {
  if (books_size(books_pool) <= 0) {
    printf("��� ����.\n");
    return;
  }

  uint64_t uid = strtoull(get_user_input("������� ����� ISBN: "), NULL, 10);
  struct book *item = books_find_by_uid(books_pool, uid);
  if (uid == 0 || item == NULL) {
    printf("������������ ����� ISBN, ���� ����� ����� �� ����������.\n");
    difficulty_1_books_view_by_uid();
    return;
  }

  printf("\n����� ISBN: %llu\n��������: %s\n������: %s\n��������: %d\n�����: %d\n\n",
         item->uid,
         item->book_name,
         item->authors,
         (int)item->available_amount,
         (int)item->total_amount);
}

void difficulty_1_books_view_all() {
  if (books_size(books_pool) <= 0) {
    printf("��� ����.\n");
    return;
  }
  for (struct book *item = books_first(books_pool); item <= books_last(books_pool); ++item) {
    if (item == NULL)
      continue;
    printf("\n����� ISBN: %llu\n��������: %s\n������: %s\n��������: %d\n�����: %d\n",
           item->uid,
           item->book_name,
           item->authors,
           (int)item->available_amount,
           (int)item->total_amount);
  }
  printf("\n");
}

void difficulty_1_books_save() {
  FILE *fp = NULL;
  fopen_s(&fp, "./books.csv", "w");
  if (fp == NULL) {
    printf("�� ������� ��������� ������ ����.\n");
    return;
  }
  books_save_csv_to_file(books_pool, fp);
  printf("����� ������� ���������.\n");
  fclose(fp);
}

void difficulty_1_books() {
  printf(
      "�������� ��������:\n1. �������� �����\n2. ������� �����\n3. ����������� ���������� �� �����\n4. ����������� ��� �����\n5. ��������� ���������\n\n0. �������\n");
  const char *input = get_user_input("��� �����: ");
  switch (*input) {
  case '1':difficulty_1_books_add();
    break;
  case '2':difficulty_1_books_remove();
    break;
  case '3':difficulty_1_books_view_by_uid();
    break;
  case '4':difficulty_1_books_view_all();
    break;
  case '5':difficulty_1_books_save();
    break;
  case '0':
    if (this_user->can_view_edit_students) {
      // Go to the previous function but do not close app if there is another permission
      return;
    }
    // Close app immediately if there's nothing other to do
    exit(0);
    break;
  }
  difficulty_1_books();
}

void difficulty_1_students_add() {
  const char *input_uid = get_user_input("������� ����� �������� ������: ");
  if (students_find_by_uid(students_pool, input_uid) != NULL) {
    printf("������� � ����� ������� �������� ������ ��� ����������.\n");
    difficulty_1_students_add();
    return;
  }

  student_insert_data data;
  // Copy all values because user input buffer is temporary, gets changed after every call of get_user_input
  data.record_book_uid = copy_string(input_uid);
  data.surname = copy_string(get_user_input("������� ������� ��������: "));
  data.name = copy_string(get_user_input("������� ��� ��������: "));
  data.patronymic = copy_string(get_user_input("������� �������� ��������: "));
  data.faculty = copy_string(get_user_input("������� ��������� ��������: "));
  data.speciality = copy_string(get_user_input("������� ������������� ��������: "));

  students_insert(students_pool, data);

  SAFE_FREE(data.surname);
  SAFE_FREE(data.name);
  SAFE_FREE(data.patronymic);
  SAFE_FREE(data.faculty);
  SAFE_FREE(data.speciality);

  printf("������� ��������.\n");
}

void difficulty_1_students_remove() {
  if (students_remove_by_uid(students_pool, get_user_input("������� ����� �������� ������: ")) == false) {
    printf("������� � ����� ������� �������� ������ �� ����������.\n");
    difficulty_1_students_remove();
    return;
  }

  printf("������� ������.\n");
}

void difficulty_1_students_edit() {
  struct student *item = students_find_by_uid(students_pool, get_user_input("������� ����� �������� ������: "));
  if (item == NULL) {
    printf("������� � ����� ������� �������� ������ �� ����������.\n");
    difficulty_1_students_edit();
    return;
  }
  const char *input = get_user_input(
      "�������� ������������� ��������:\n1.�������\n2. ���\n3. ��������\n4. ���������\n5. �������������\n\n0. �����\n\n��� �����: ");
  char selected = *input;
  if (!(selected >= '1' && selected <= '5')) {
    if (selected != '0')
      printf("�� ������� ������������� �����.\n");
    return;
  }
  const char *new_value = copy_string(get_user_input("������� ����� ��������: "));
  switch (selected) {
  case '1': {
    SAFE_FREE(item->surname);
    item->surname = new_value;
    break;
  }
  case '2': {
    SAFE_FREE(item->name);
    item->name = new_value;
    break;
  }
  case '3': {
    SAFE_FREE(item->patronymic);
    item->patronymic = new_value;
    break;
  }
  case '4': {
    SAFE_FREE(item->faculty);
    item->faculty = new_value;
    break;
  }
  case '5': {
    SAFE_FREE(item->speciality);
    item->speciality = new_value;
    break;
  }
  }
}

void difficulty_1_students_view_by_uid() {
  struct student *item = students_find_by_uid(students_pool, get_user_input("������� ����� �������� ������: "));
  if (item == NULL) {
    printf("������� � ����� ������� �������� ������ �� ����������.\n");
    difficulty_1_students_view_by_uid();
    return;
  }

  printf("\n����� �������� ������: %s\n�������: %s\n���: %s\n��������: %s\n���������: %s\n�������������: %s\n\n",
         item->record_book_uid,
         item->surname,
         item->name,
         item->patronymic,
         item->faculty,
         item->speciality);
}

void difficulty_1_students_save() {
  FILE *fp = NULL;
  fopen_s(&fp, "./students.csv", "w");
  if (fp == NULL) {
    printf("�� ������� ��������� ������ ���������.\n");
    return;
  }
  students_save_csv_to_file(students_pool, fp);
  printf("�������� ������� ���������.\n");
  fclose(fp);
}

void difficulty_1_students() {
  printf(
      "�������� ��������:\n1. �������� ��������\n2. ������� ��������\n3. ������������� ��������\n4. ����������� ���������� � ��������\n5. ��������� ���������\n\n0. �������\n");
  const char *input = get_user_input("��� �����: ");
  switch (*input) {
  case '1':difficulty_1_students_add();
    break;
  case '2':difficulty_1_students_remove();
    break;
  case '3':difficulty_1_students_edit();
    break;
  case '4':difficulty_1_students_view_by_uid();
    break;
  case '5':difficulty_1_students_save();
    break;
  case '0':
    if (this_user->can_view_edit_books) {
      // Go to the previous function but do not close app if there is another permission
      return;
    }
    // Close app immediately if there's nothing other to do
    exit(0);
    break;
  }
  difficulty_1_students();
}

void difficulty_2_actions_both() {
  printf("�������� ���������:\n1. �����\n2. ��������\n\n0. �������\n");
  const char *input = get_user_input("��� �����: ");
  switch (*input) {
  case '1':difficulty_1_books();
    break;
  case '2':difficulty_1_students();
    break;
  case '0':exit(0);
    break;
  }
  difficulty_2_actions_both();
}

void difficulty_2() {
  ask_for_auth();
  if (this_user->can_view_edit_books && this_user->can_view_edit_students) {
    difficulty_2_actions_both();
  } else if (this_user->can_view_edit_books) {
    difficulty_1_books();
  } else if (this_user->can_view_edit_students) {
    difficulty_1_students();
  } else {
    pause_and_exit("� ��� ������������ ���� ��� ������� ���������. ��������� ����� �������.\n", 0);
  }
}

int main() {
  books_pool = parse_books();
  students_pool = parse_students();
  users_pool = parse_users();

  /* All I/O and this file encoding must be CP-1251,
   * otherwise we will notice encoding faults in output data and files
   */
  SetConsoleCP(1251);
  SetConsoleOutputCP(1251);

  if (books_pool == NULL) {
    pause_and_exit(
        "���� ������ ���� �� �������.\n����������, �������� ��������������� ���� books.csv � ����� � ����������.\n��������� ����� �������.\n",
        1);
  }

  if (students_pool == NULL) {
    pause_and_exit(
        "���� ������ ��������� �� �������.\n����������, �������� ��������������� ���� students.csv � ����� � ����������.\n��������� ����� �������.\n",
        2);
  }

  if (users_pool == NULL) {
    pause_and_exit(
        "���� ������ ������������� �� �������.\n����������, �������� ��������������� ���� users.csv � ����� � ����������.\n��������� ����� �������.\n",
        3);
  }

  while (true)
    difficulty_2();

  return 0;
}
