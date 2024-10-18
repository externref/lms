#include <ctype.h>
#include <sqlite3.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define string char *
#define BOOK_LIMIT 1000
#define USER_LIMIT 1000

struct Book {
  int id;
  string name;
  string category;
  string author;
  int year;
};

struct User {
  int id;
  string name;
  string address;
  long int phone;
  int yob;
};

struct Borrow {
  long int user_id;
  long int book_id;
  long int borrow_timestamp;
};

sqlite3 *db;

struct Book books[BOOK_LIMIT];
int total_books = 0;
int selected_books = 0;

static int load_books_callback(void *NotUsed, int argc, char **argv,
                               char **azColName) {
  if (total_books >= BOOK_LIMIT) {
    printf("Book limit reached. Cannot load more books.\n");
    return 0;
  }
  books[total_books].id = atoi(argv[0]);
  books[total_books].name = strdup(argv[1]);
  books[total_books].category = strdup(argv[2]);
  books[total_books].author = strdup(argv[3]);
  books[total_books].year = atoi(argv[4]);
  total_books++;

  printf("%s\n", books[total_books - 1].name);
  return 0;
}

void d_setups(sqlite3 *database) {
  char *err = NULL;
  FILE *fileptr;
  char sql[500];

  fileptr = fopen("../setups.sql", "r");
  if (fileptr == NULL) {
    printf("Error: Could not open setups.sql file.\n");
    return;
  }

  fgets(sql, sizeof(sql), fileptr);
  fclose(fileptr);

  int rc = sqlite3_exec(database, sql, NULL, 0, &err);
  if (rc != SQLITE_OK) {
    printf("SQLite Error: %s\n", err);
    sqlite3_free(err);
  } else {
    printf("Tables created successfully.\n");
  }

  const char *select_books_sql = "SELECT * FROM books;";
  rc = sqlite3_exec(database, select_books_sql, load_books_callback, 0, &err);

  if (rc != SQLITE_OK) {
    printf("SQLite Error while loading books: %s\n", err);
    sqlite3_free(err);
  } else {
    printf("Loaded %d books from the database.\n", total_books);
  }

  db = database;
}

void to_lowercase(char *str) {
  for (int i = 0; str[i]; i++) {
    str[i] = tolower((unsigned char)str[i]);
  }
}

int starts_with_case_insensitive(const char *book_name, const char *query) {
  char book_name_lower[256];
  char query_lower[256];

  strncpy(book_name_lower, book_name, sizeof(book_name_lower));
  strncpy(query_lower, query, sizeof(query_lower));

  to_lowercase(book_name_lower);
  to_lowercase(query_lower);

  return strncmp(book_name_lower, query_lower, strlen(query_lower)) == 0;
}

int compare_books(struct Book b1, struct Book b2, char *sort_by) {
  if (strcmp(sort_by, "name") == 0) {
    return strcmp(b1.name, b2.name);
  } else if (strcmp(sort_by, "author") == 0) {
    return strcmp(b1.author, b2.author);
  } else if (strcmp(sort_by, "year") == 0) {
    return b1.year - b2.year;
  }
  return 0;
}

void merge(struct Book arr[], int left, int mid, int right, char *sort_by) {
  int i, j, k;
  int n1 = mid - left + 1;
  int n2 = right - mid;

  struct Book *L = malloc(n1 * sizeof(struct Book));
  struct Book *R = malloc(n2 * sizeof(struct Book));

  for (i = 0; i < n1; i++)
    L[i] = arr[left + i];
  for (j = 0; j < n2; j++)
    R[j] = arr[mid + 1 + j];

  i = 0;
  j = 0;
  k = left;
  while (i < n1 && j < n2) {
    if (compare_books(L[i], R[j], sort_by) <= 0) {
      arr[k] = L[i];
      i++;
    } else {
      arr[k] = R[j];
      j++;
    }
    k++;
  }

  while (i < n1) {
    arr[k] = L[i];
    i++;
    k++;
  }

  while (j < n2) {
    arr[k] = R[j];
    j++;
    k++;
  }

  free(L);
  free(R);
}

void merge_sort(struct Book arr[], int left, int right, char *sort_by) {
  if (left < right) {
    int mid = left + (right - left) / 2;
    merge_sort(arr, left, mid, sort_by);
    merge_sort(arr, mid + 1, right, sort_by);
    merge(arr, left, mid, right, sort_by);
  }
}

struct Book *d_get_books(char *query, char *sort_by) {
  struct Book *f_books = malloc(BOOK_LIMIT * sizeof(struct Book));
  int count = 0;

  for (int i = 0; i < total_books; i++) {
    if (query == NULL || (strlen(query) > 0 &&
                          starts_with_case_insensitive(books[i].name, query))) {
      f_books[count] = books[i];
      count++;
    }
  }

  if (count > 0) {
    merge_sort(f_books, 0, count - 1, sort_by);
  }

  return f_books;
}
