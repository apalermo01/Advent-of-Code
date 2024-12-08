#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF_LENGTH 2147483

struct array2d {
  char *arr;
  int numrows;
  int numcols;
};

struct line_info {
  int num_lines;
  int max_width;
};

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
    printf("OUT OF BOUNDS\n");
    return 1;
  }

  if (col >= arr.numcols) {
    printf("OUT OF BOUNDS\n");
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

void get_substring_vert(char *dest, const struct array2d arr, int row, int col,
                        int num_chars) {
  for (int i = 0; i < num_chars; i++) {
    dest[i] = get_value(arr, row + i, col, 0);
  }
}

int check_horizontal(struct array2d array) {
  int result = 0;
  for (int i = 0; i < array.numrows; i++) {
    /*printf("row = %d\n", i);*/
    /*print_row(array, i);*/
    /*printf("\n");*/

    for (int j = 0; j < array.numcols - 3; j++) {
      if (get_value(array, i, j    , 0) == 'X' &&
          get_value(array, i, j + 1, 0) == 'M' &&
          get_value(array, i, j + 2, 0) == 'A' &&
          get_value(array, i, j + 3, 0) == 'S') {
        result++;
      }
      if (get_value(array, i, j    , 0) == 'S' &&
          get_value(array, i, j + 1, 0) == 'A' &&
          get_value(array, i, j + 2, 0) == 'M' &&
          get_value(array, i, j + 3, 0) == 'X') {
        result++;
      }
    }
  }

  return result;
}

int check_vertical(struct array2d array) {
  int result = 0;
  for (int i = 0; i < array.numrows - 3; i++) {
    for (int j = 0; j < array.numcols; j++) {
      if (get_value(array, i, j    , 0) == 'X' &&
          get_value(array, i + 1, j, 0) == 'M' &&
          get_value(array, i + 2, j, 0) == 'A' &&
          get_value(array, i + 3, j, 0) == 'S') {
        result++;
      }
      if (get_value(array, i, j    , 0) == 'S' &&
          get_value(array, i + 1, j, 0) == 'A' &&
          get_value(array, i + 2, j, 0) == 'M' &&
          get_value(array, i + 3, j, 0) == 'X') {
        result++;
      }
    }
  }

  return result;
}

int check_diag(struct array2d array) {
  int result = 0;
  for (int i = 0; i < array.numrows; i++) {
    /*printf("row in checking diag = ");*/
    /*print_row(array, i);*/
    /*printf("\n");*/
    for (int j = 0; j < array.numcols; j++) {
      // down and right
      if (i <= array.numrows - 3 && j <= array.numcols - 3 &&
          get_value(array, i, j        , 0) == 'X' &&
          get_value(array, i + 1, j + 1, 0) == 'M' &&
          get_value(array, i + 2, j + 2, 0) == 'A' &&
          get_value(array, i + 3, j + 3, 0) == 'S') {
        /*printf("down and right match at %d, %d\n", i, j);*/
        result++;
      }

      // down and left
      /*if (j == array.numcols-1  ) {*/
      /*  printf("%c\n", get_value(array, i, j));*/
      /*}*/
      if (i <= array.numrows - 3 &&
          j >= 3 &&
          get_value(array, i, j        , 0) == 'X' &&
          get_value(array, i + 1, j - 1, 0) == 'M' &&
          get_value(array, i + 2, j - 2, 0) == 'A' &&
          get_value(array, i + 3, j - 3, 0) == 'S') {
        /*printf("down and left match at %d, %d\n", i, j);*/
        result++;
      }

      // up and right
      if (i >= 3 &&
          j <= array.numcols - 3 &&
          get_value(array, i, j        , 0) == 'X' &&
          get_value(array, i - 1, j + 1, 0) == 'M' &&
          get_value(array, i - 2, j + 2, 0) == 'A' &&
          get_value(array, i - 3, j + 3, 0) == 'S') {
        /*printf("up and right match at %d, %d\n", i, j);*/
        result++;
      }

      // up and left
      if (i >= 3 &&
          j >= 3 && 
          get_value(array, i, j        , 0) == 'X' &&
          get_value(array, i - 1, j - 1, 0) == 'M' &&
          get_value(array, i - 2, j - 2, 0) == 'A' &&
          get_value(array, i - 3, j - 3, 0) == 'S') {
        /*printf("up and left match at %d, %d\n", i, j);*/
        result++;
      }
    }
  }

  return result;
}
int solve_v1(struct array2d array) {
  int result = 0;
  result += check_horizontal(array);
  result += check_vertical(array);
  result += check_diag(array);
  return result;
}

int solve_v2(struct array2d array) {
  printf("solving using version 2\n");
  int result = 0;
  get_value(array, array.numrows, array.numcols, 0);
  for (int i = 0; i < array.numrows; i++) {
    for (int j = 0; j < array.numcols; j++) {

      if (get_value(array, i, j, 0) == 'X') {

        // forward
        if(j+3 <= array.numcols &&
           get_value(array, i, j+1, 0) == 'M' &&
           get_value(array, i, j+2, 0) == 'A' &&
           get_value(array, i, j+3, 0) == 'S') {result ++;}

        // backward
        if(j >= 3 &&
           get_value(array, i, j-1, 0) == 'M' &&
           get_value(array, i, j-2, 0) == 'A' &&
           get_value(array, i, j-3, 0) == 'S') {result ++;}

        // down
        if(i+3 <= array.numrows &&
           get_value(array, i + 1, j, 0) == 'M' &&
           get_value(array, i + 2, j, 0) == 'A' &&
           get_value(array, i + 3, j, 0) == 'S') {result ++;}

        // up
        if(i >= 3 &&
           get_value(array, i - 1, j, 0) == 'M' &&
           get_value(array, i - 2, j, 0) == 'A' &&
           get_value(array, i - 3, j, 0) == 'S') {result ++;}

        // down and right
        if (i+3 <= array.numrows &&
            j+3 <= array.numcols &&
            get_value(array, i + 1, j + 1, 0) == 'M' &&
            get_value(array, i + 2, j + 2, 0) == 'A' &&
            get_value(array, i + 3, j + 3, 0) == 'S') {
          /*printf("down and right match at %d, %d\n", i, j);*/
          result++;
        }

        // down and left
        if (i+3 <= array.numrows &&
            j >= 3 &&
            get_value(array, i + 1, j - 1, 0) == 'M' &&
            get_value(array, i + 2, j - 2, 0) == 'A' &&
            get_value(array, i + 3, j - 3, 0) == 'S') {
          /*printf("down and left match at %d, %d\n", i, j);*/
          result++;
        }

        // up and right
        if (i >= 3 &&
            j+3 <= array.numcols &&
            get_value(array, i - 1, j + 1, 0) == 'M' &&
            get_value(array, i - 2, j + 2, 0) == 'A' &&
            get_value(array, i - 3, j + 3, 0) == 'S') {
          /*printf("up and right match at %d, %d\n", i, j);*/
          result++;
        }

        // up and left
        if (i >= 3 &&
            j >= 3 && 
            get_value(array, i - 1, j - 1, 0) == 'M' &&
            get_value(array, i - 2, j - 2, 0) == 'A' &&
            get_value(array, i - 3, j - 3, 0) == 'S') {
          /*printf("up and left match at %d, %d\n", i, j);*/
          result++;
        }
      }
    }
  }

  return result;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: ./main input_file problem_section (1 or 2)\n");
    exit(1);
  }

  if (strnlen(argv[1], MAX_BUF_LENGTH) >= MAX_BUF_LENGTH) {
    printf("input exceeds max length of %d", MAX_BUF_LENGTH);
    exit(1);
  }

  FILE *fptr;

  printf("opening file %s\n", argv[1]);
  if ((fptr = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error opening file");
    exit(1);
  }

  char mode_char = *argv[2];

  if (!isdigit(mode_char)) {
    fprintf(stderr, "second argument must be 1 or 2 \n");
    free(fptr);
    exit(1);
  }

  int mode = string_to_num(&mode_char);
  if ((mode != 1) && (mode != 2)) {
    fprintf(stderr, "second argument must be a 1 or 2");
    free(fptr);
    exit(1);
  }

  int total = 0;

  ssize_t read;
  size_t len = 0;
  char *line = NULL;

  // figure out how many lines there are
  struct line_info linfo = count_lines(fptr);

  // rewind the file back to the beginning
  rewind(fptr);

  // make a 2d array of all the characters
  struct array2d array = file_to_array(fptr, linfo);
  if (mode == 1) {
    total = solve_v1(array);
  }

  if (mode == 2) {
    total = solve_v2(array);
  }
  printf("total is %d\n", total);
}

// 2564
// 2552
// 2554
// 2565
// 2566
