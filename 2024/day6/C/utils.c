
#include <errno.h>
#include <regex.h>
#include <stdio.h>
#include <stdlib.h>

#define MAX_BUF_LENGTH 2147483

int string_to_num(const char *str) {

  char *endptr;
  errno = 0;
  long num = strtol(str, &endptr, 10);

  if (errno == EINVAL) {
    perror("strtol");
    return 0;
  }
  return (int)num;
}

int get_num_from_regmatch(const regmatch_t regmatch, const char *s,
                          const char *line) {

  regoff_t off, len;

  off = regmatch.rm_so + (s - line);
  len = regmatch.rm_eo - regmatch.rm_so;

  char *str_num = malloc((len + 1) * sizeof(char));

  if (str_num == NULL) {
    fprintf(stderr, "memory allocation failed\n");
    return 0;
  }
  for (int i = 0; i < len; i++) {
    str_num[i] = line[off + i];
  }
  str_num[len] = '\0';

  int num = string_to_num(str_num);

  free(str_num);

  return num;
}

struct array2d {
  char *arr;
  int numrows;
  int numcols;
};

struct line_info {
  int num_lines;
  int max_width;
};

/*
 * adapted from: https://stackoverflow.com/a/70708991
 * Counts the number of lines in a file and the maximum
 * width of a line
 */
struct line_info count_lines(FILE *file) {
  char buf[MAX_BUF_LENGTH];
  int row_counter = 0;
  int max_width = 0;
  int width_counter = 0;

  struct line_info ret;

  for (;;) {
    size_t res = fread(buf, 1, MAX_BUF_LENGTH, file);
    if (ferror(file)) {
      ret.num_lines = -1;
      ret.max_width = -1;
      return ret;
    }

    int i;
    for (i = 0; i < res; i++) {
      if (buf[i] == '\n') {
        row_counter++;
        if (width_counter > max_width) {
          max_width = width_counter;
        }
        width_counter = 0;
      } else {
        width_counter++;
      }
    }

    if (feof(file)) {
      break;
    }
  }

  ret.num_lines = row_counter;
  ret.max_width = max_width;
  return ret;
}

struct array2d make_array(int nrows, int ncols) {
  char *array = malloc(sizeof(char) * nrows * ncols);
  struct array2d new_array;
  new_array.arr = array;
  new_array.numrows = nrows;
  new_array.numcols = ncols;

  return new_array;
}

int set_in_array(struct array2d arr, int row, int col, char value) {

  // bounds-checking
  if (row >= arr.numrows) {
    printf("OUT OF BOUNDS. Array has %d rows, tried to set row %d\n",
           arr.numrows, row);
    return 1;
  }

  if (col >= arr.numcols) {
    printf("OUT OF BOUNDS. Array has %d columns, tried to set column %d\n",
           arr.numcols, col);
    return 1;
  }

  // set the value
  arr.arr[(arr.numcols * row) + col] = value;

  // return 0 for success
  return 0;
}

int get_value(struct array2d arr, int row, int col, int p) {
  /*if (p == 1) {*/
  /*  printf("getting index %d\n", (arr.numcols * row) + col);*/
  /*}*/
  return arr.arr[(arr.numcols * row) + col];
}

void freearr(struct array2d *arr) { free(arr->arr); }

void print_row(struct array2d array, int row) {
  if (row >= array.numrows) {
    fprintf(stderr, "ERROR: row %d is out of bounds for array with %d rows\n",
            row, array.numrows);
    return;
  }
  for (int j = 0; j < array.numcols; j++) {
    printf("%c", get_value(array, row, j, 1));
  }
}

void get_row(struct array2d array, int row, char *buf, size_t buffsize) {
  if (buffsize < array.numcols) {
    fprintf(stderr, "get_row: passed buffer is too small for array's row size");
  }
  for (int j = 0; j < array.numcols; j++) {
    buf[j] = get_value(array, row, j, 0);
  }
}

struct array2d file_to_array(FILE *file, struct line_info linfo) {
  int nrows = linfo.num_lines;
  int ncols = linfo.max_width;
  char *line = NULL;
  size_t len = 0;
  ssize_t read;

  int thisrow = 0;
  int thiscol = 0;
  struct array2d arr = make_array(nrows, ncols);

  while ((read = getline(&line, &len, file)) != -1) {
    /*printf("row          = %s", line);*/
    for (thiscol = 0; thiscol < ncols; thiscol++) {
      set_in_array(arr, thisrow, thiscol, line[thiscol]);
    }
    /*printf("row in array = ");*/
    /*print_row(arr, thisrow);*/
    /*printf("\n");*/
    thisrow++;
  }

  /*printf("\n");*/
  /*printf("\n");*/
  /*printf("\n");*/
  /*for (int i = 0; i < thisrow; i++) {*/
  /*  printf("row in array after write = ");*/
  /*  print_row(arr, i);*/
  /*  printf("\n");*/
  /*}*/

  return arr;
}

void get_substring(char *dest, const struct array2d arr, int row, int col,
                   int num_chars) {
  for (int i = 0; i < num_chars; i++) {
    dest[i] = get_value(arr, row, col + i, 0);
  }
}
