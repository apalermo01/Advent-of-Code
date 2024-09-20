#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUF_SIZE 65536
#define MAX_DIGITS 4

// define a structure for returning a tuple that represents the information we
// need to grab from the file before doing anything else
struct line_info {
  int num_lines;
  int max_width;
};

/* define a structure that represents a 2d array. The objective here is to make
 * indexing as easy as possible.
 *
 * The array will actually be stored as a 1-dimensional array. The 2d aspect
 * will all come from the indexing logic.
 */
struct array2d {
  char *arr;
  int numrows;
  int numcols;
};

int string_to_num(char *str) {
  int total = 0;
  int pow = 1;

  int num_digits = strnlen(str, 8);
  int num;

  for (int i = num_digits; i >= 0; i--) {
    if (str[i] == '\0')
      continue;
    num = str[i] - '0';
    total += pow * num;
    pow *= 10;
  }

  return total;
}
/*
 * adapted from: https://stackoverflow.com/a/70708991
 * Counts the number of lines in a file and the maximum
 * width of a line
 */
struct line_info count_lines(FILE *file) {
  char buf[BUF_SIZE];
  int row_counter = 0;
  int max_width = 0;
  int width_counter = 0;

  struct line_info ret;

  for (;;) {
    size_t res = fread(buf, 1, BUF_SIZE, file);
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

/* initialization logic for array */
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
  arr.arr[arr.numcols * col + row] = value;

  // return 0 for success
  return 0;
}

int get_value(struct array2d arr, int row, int col) {
  return arr.arr[arr.numcols * col + row];
}

void freearr(struct array2d *arr) { free(arr->arr); }

/*
 * Take a file and convert it to a 2d array
 */
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
    for (thiscol = 0; thiscol < ncols; thiscol++) {
      set_in_array(arr, thisrow, thiscol, line[thiscol]);
    }
    thisrow++;
  }

  return arr;
}

void print_row(struct array2d array, int row) {
  if (row >= array.numrows) {
    fprintf(stderr, "ERROR: row %d is out of bounds for array with %d rows\n",
            row, array.numrows);
  }
  for (int j = 0; j < array.numcols; j++) {
    printf("%c", get_value(array, row, j));
  }
}

void get_row(struct array2d array, int row, char *buf, size_t buffsize) {
  if (buffsize > array.numcols) {
    fprintf(stderr, "get_row: passed buffer is too small for array's row size");
  }
  for (int j = 0; j < array.numcols; j++) {
    buf[j] = get_value(array, row, j);
  }
}

struct array2d pad_array(struct array2d arr) {
  int nrows = arr.numrows + 2;
  int ncols = arr.numcols + 2;

  struct array2d new_array = make_array(nrows, ncols);

  // original text
  for (int i = 1; i < nrows - 1; i++) {
    for (int j = 1; j < ncols - 1; j++) {
      set_in_array(new_array, i, j, get_value(arr, i - 1, j - 1));
    }
  }

  // upper row and lower rows
  for (int j = 0; j < ncols; j++) {
    if (set_in_array(new_array, 0, j, '.') != 0) {
      fprintf(stderr, "error assigning value: row=%d, col=%d\n", 0, j);
    }

    if (set_in_array(new_array, nrows - 1, j, '.') != 0) {
      fprintf(stderr, "error assigning value: row=%d, col=%d\n", nrows, j);
    }
  }

  // left and right columns
  for (int i = 0; i < nrows - 1; i++) {

    if (set_in_array(new_array, i, 0, '.') != 0) {
      fprintf(stderr, "error assigning value: row=%d, col=%d\n", i, 0);
    }
    if (set_in_array(new_array, i, ncols - 1, '.') != 0) {
      fprintf(stderr, "error assigning value: row=%d, col=%d\n", i, ncols);
    }
  }

  return new_array;
}

/*
 * Given some buffer and starting at the position indicated by starting_pos,
 * iterate backwards through the buffer until we hit a non-number,
 * then concatenate the resulting characters to a number and return it.
 */
int backward_search(char *buf, int starting_pos, int max_digits,
                    size_t buffsize) {

  /*printf("running backward search\n");*/
  char *found_digits_tmp = malloc(max_digits * sizeof(char));
  int num_digits_found = 0;

  if (found_digits_tmp == NULL) {
    fprintf(stderr, "Error allocating memory for backward search");
    return 0;
  }

  for (int offset = 0; offset < max_digits; offset++) {
    if (isdigit(buf[starting_pos - offset]) == 0 || starting_pos - offset < 0) {
      break;
    }
    found_digits_tmp[max_digits - offset - 1] = buf[starting_pos - offset];
    num_digits_found++;
  }

  char *found_digits = malloc((num_digits_found + 1) * sizeof(char));
  if (found_digits == NULL) {
    free(found_digits_tmp);
    fprintf(stderr, "Error allocating memory for backward search");
    return 0;
  }

  for (int i = 0; i < num_digits_found; i++) {
    found_digits[i] = found_digits_tmp[max_digits - num_digits_found + i];
  }
  found_digits[num_digits_found] = '\0';

  int ret = string_to_num(found_digits);

  free(found_digits_tmp);
  free(found_digits);

  return ret;
}

/*
 * Given some buffer and starting at the position indicated by starting_pos,
 * iterate forwards through the buffer until we hit a non-number,
 * then concatenate the resulting characters to a number and return it.
 */
int forward_search(char *buf, int starting_pos, int max_digits,
                   size_t buffsize) {
  char *found_digits_tmp = malloc(max_digits * sizeof(char));
  int num_digits_found = 0;

  if (found_digits_tmp == NULL) {
    fprintf(stderr, "Error allocating memory for forward search");
    return 0;
  }

  for (int offset = 0; offset < max_digits; offset++) {
    if (isdigit(buf[starting_pos + offset]) == 0 ||
        starting_pos + offset - 1 > buffsize) {
      break;
    }
    found_digits_tmp[offset] = buf[starting_pos + offset];
    num_digits_found++;
  }

  char *found_digits = malloc((num_digits_found + 1) * sizeof(char));
  if (found_digits == NULL) {
    free(found_digits_tmp);
    fprintf(stderr, "Error allocating memory for forward search");
    return 0;
  }

  for (int i = 0; i < num_digits_found; i++) {
    found_digits[i] = found_digits_tmp[i];
  }
  found_digits[num_digits_found] = '\0';

  int ret = string_to_num(found_digits);

  free(found_digits);
  free(found_digits_tmp);

  return ret;
}

/*
 * Given some buffer and starting at the position indicated by starting_pos,
 * iterate forwards through the buffer until we hit a non-number, then iterate
 * backwards until we hit a non-number, then concatenate the resulting
 * characters to a number and return it.
 */
int middle_search(char *buf, int starting_pos, int max_digits,
                  size_t buffsize) {
  char *found_digits_tmp = malloc((max_digits * 2 + 1) * sizeof(char));
  int num_digits_found = 0;

  if (found_digits_tmp == NULL) {
    fprintf(stderr, "Error allocating memory for forward search");
    return 0;
  }

  // forward
  int offset;
  for (offset = 0; offset < max_digits; offset++) {
    if (isdigit(buf[starting_pos + offset]) == 0 ||
        starting_pos + offset - 1 > buffsize) {
      break;
    }

    found_digits_tmp[max_digits + offset] = buf[starting_pos + offset];
    num_digits_found++;
  }

  // backward
  for (offset = -1; offset > max_digits * -1; offset--) {
    if (isdigit(buf[starting_pos + offset]) == 0 || starting_pos + offset < 0) {
      break;
    }

    found_digits_tmp[max_digits + offset] = buf[starting_pos + offset];
    num_digits_found++;
  }

  char *found_digits = malloc((num_digits_found + 1) * sizeof(char));
  if (found_digits == NULL) {
    free(found_digits_tmp);
    return 0;
  }

  for (int i = 0; i < num_digits_found; i++) {
    found_digits[i] = found_digits_tmp[offset + max_digits + i + 1];
  }
  found_digits[num_digits_found] = '\0';
  int ret = string_to_num(found_digits);

  free(found_digits);
  free(found_digits_tmp);

  return ret;
}
/*
 * core logic for problem 1
 * Objective: the 2d array has a collection of numbers and symbols
 * A part number is defined as any number adjacent to a symbol. (
 * A symbol is anything that is not a digit or '.'). We want to
 * add up all the part numbers in the array
 */

int get_part_numbers(struct array2d text, int row, int col, int mode) {
  /*
   * We're searching around the text array like this:
   *
   * ? ? ?
   * ? * ?
   * ? ? ?
   *
   * where * is located at (row, col) in text and we need to check if ?
   * is a number. If ? is a number, we need to recover and concatenate together
   * the other numbers. Here I'm going to list out all the cases we need to
   * check for. The top and bottom row have similar logic so I'll count those as
   * the same cases.
   *
   * 1:
   * # . .
   * ? * ?
   *
   * 2:
   * . # .
   * ? * ?
   *
   * 3:
   * . . #
   * ? * ?
   *
   * 4:
   * # # .
   * ? * ?
   *
   * 5:
   * . # #
   * ? * ?
   *
   * 6:
   * # . #
   * ? * ?
   *
   * 7:
   * ? ? ?
   * # * #
   */
   if (mode != 1 && mode != 2) {
    printf("invalid mode, options are 1 or 2");
    return 0;
  }

  int numbers[255];

  for (int i = 0; i < 255; i++) {
    numbers[i] = 0;
  }

  int num_count = 0;

  // create an array of bools that describe which case(s) we're dealing with
  int top_cases[] = {isdigit(get_value(text, row - 1, col - 1)),
                     isdigit(get_value(text, row - 1, col)),
                     isdigit(get_value(text, row - 1, col + 1))};
  int middle_cases[] = {isdigit(get_value(text, row, col - 1)),
                        isdigit(get_value(text, row, col + 1))};
  int bottom_cases[] = {isdigit(get_value(text, row + 1, col - 1)),
                        isdigit(get_value(text, row + 1, col)),
                        isdigit(get_value(text, row + 1, col + 1))};

  char *top_line = malloc(text.numcols * sizeof(char));
  char *middle_line = malloc(text.numcols * sizeof(char));
  char *bottom_line = malloc(text.numcols * sizeof(char));

  get_row(text, row - 1, top_line, text.numcols);
  get_row(text, row, middle_line, text.numcols);
  get_row(text, row + 1, bottom_line, text.numcols);

  if (top_line == NULL || middle_line == NULL || bottom_line == NULL) {
    free(top_line);
    free(middle_line);
    free(bottom_line);
    fprintf(stderr, "Error allocating memory for lines, returning 0");
    return 0;
  }

  /*
   * case 1:
   *
   * # . .     ? ? ?
   * ? * ? or  ? * ?
   * ? ? ?     # . .
   */
  if (top_cases[0] > 0 && top_cases[1] == 0 && top_cases[2] == 0) {
    numbers[num_count] =
        backward_search(top_line, col - 1, MAX_DIGITS, text.numcols);
    num_count++;
  }
  if (bottom_cases[0] > 0 && bottom_cases[1] == 0 && bottom_cases[2] == 0) {
    numbers[num_count] =
        backward_search(bottom_line, col - 1, MAX_DIGITS, text.numcols);
    num_count++;
  }

  /*
   * case 2:
   *
   * . # .    ? ? ?
   * ? * ? or ? * ?
   * ? ? ?    . # .
   */
  if (top_cases[0] == 0 && top_cases[1] > 0 && top_cases[2] == 0) {
    numbers[num_count] = top_line[col] - '0';
    num_count++;
  }
  if (bottom_cases[0] == 0 && bottom_cases[1] > 0 && bottom_cases[2] == 0) {
    numbers[num_count] = bottom_line[col] - '0';
    num_count++;
  }

  /*
   * case 3:
   *
   * . . #    ? ? ?
   * ? * ? or ? * ?
   * ? ? ?    . . #
   */
  if (top_cases[0] == 0 && top_cases[1] == 0 && top_cases[2] > 0) {
    numbers[num_count] =
        forward_search(top_line, col + 1, MAX_DIGITS, text.numcols);
    num_count++;
  }
  if (bottom_cases[0] == 0 && bottom_cases[1] == 0 && bottom_cases[2] > 0) {
    numbers[num_count] =
        forward_search(bottom_line, col + 1, MAX_DIGITS, text.numcols);
    num_count++;
  }

  /*
   * case 4:
   *
   * # # .    ? ? ?
   * ? * ? or ? * ?
   * ? ? ?    # # .
   */
  if (top_cases[0] > 0 && top_cases[1] > 0 && top_cases[2] == 0) {
    numbers[num_count] =
        backward_search(top_line, col, MAX_DIGITS, text.numcols);
    num_count++;
  }
  if (bottom_cases[0] > 0 && bottom_cases[1] > 0 && bottom_cases[2] == 0) {
    numbers[num_count] =
        backward_search(bottom_line, col, MAX_DIGITS, text.numcols);
    num_count++;
  }

  /*
   * case 5:
   *
   * . # #    ? ? ?
   * ? * ? or ? * ?
   * ? ? ?    . # #
   */
  if (top_cases[0] == 0 && top_cases[1] > 0 && top_cases[2] > 0) {
    numbers[num_count] =
        forward_search(top_line, col, MAX_DIGITS, text.numcols);
    num_count++;
  }
  if (bottom_cases[0] == 0 && bottom_cases[1] > 0 && bottom_cases[2] > 0) {
    numbers[num_count] =
        forward_search(bottom_line, col, MAX_DIGITS, text.numcols);
    num_count++;
  }

  /*
   * case 6:
   *
   * # . #    ? ? ?
   * ? * ? or ? * ?
   * ? ? ?    # . #
   */
  if (top_cases[0] > 0 && top_cases[1] == 0 && top_cases[2] > 0) {
    numbers[num_count] =
        backward_search(top_line, col - 1, MAX_DIGITS, text.numcols);
    num_count++;
    numbers[num_count] =
        forward_search(top_line, col + 1, MAX_DIGITS, text.numcols);
    num_count++;
  }
  if (bottom_cases[0] > 0 && bottom_cases[1] == 0 && bottom_cases[2] > 0) {
    numbers[num_count] =
        backward_search(bottom_line, col - 1, MAX_DIGITS, text.numcols);
    num_count++;
    numbers[num_count] =
        forward_search(bottom_line, col + 1, MAX_DIGITS, text.numcols);
    num_count++;
  }

  /*
   * case 7:
   *
   * # # #    ? ? ?
   * ? * ? or ? * ?
   * ? ? ?    # # #
   */
  if (top_cases[0] > 0 && top_cases[1] > 0 && top_cases[2] > 0) {
    numbers[num_count] = middle_search(top_line, col, MAX_DIGITS, text.numcols);
    num_count++;
  }
  if (bottom_cases[0] > 0 && bottom_cases[1] > 0 && bottom_cases[2] > 0) {
    numbers[num_count] =
        middle_search(bottom_line, col, MAX_DIGITS, text.numcols);
    num_count++;
  }

  /*
   * case 8:
   *
   * middle line has numbers
   */

  if (middle_cases[0] > 0) {
    numbers[num_count] =
        backward_search(middle_line, col - 1, MAX_DIGITS, text.numcols);
    num_count++;
  }
  if (middle_cases[1] > 0) {
    numbers[num_count] =
        forward_search(middle_line, col + 1, MAX_DIGITS, text.numcols);
    num_count++;
  }

  free(top_line);
  free(middle_line);
  free(bottom_line);

  if (mode == 1) {
    int total = 0;
    for (int i = 0; i < num_count; i++) {
      total += numbers[i];
    }
    return total;
  } else {
    if (num_count == 2) {
      return numbers[0] * numbers[1];
    } else {
      return 0;
    }
  }
}

int solve(struct array2d text, int mode) {
  int part_total = 0;
  
  if (mode == 1) {
    printf("finding part numbers\n");
  } else if (mode == 2) {
    printf("finding gear ratios\n");
  } else {
    printf("invalid mode\n");
    return 0;
  }

  // pad the array so we can treat everything equally
  text = pad_array(text);
  char value;

  // loop through the "core" of the file
  for (int i = 1; i < text.numrows - 1; i++) {
    for (int j = 1; j < text.numcols - 1; j++) {
      value = get_value(text, i, j);
      if (value != '.' && isdigit(value) == 0) {
        part_total += get_part_numbers(text, i, j, mode);
      }
    }
  }
  return part_total;
}

int main() {
  FILE *fptr;

  /*if ((fptr = fopen("../sample_input_2.txt", "r")) == NULL) {*/
  if ((fptr = fopen("../input_1.txt", "r")) == NULL) {
    fprintf(stderr, "Error opening file");
    free(fptr);
    exit(1);
  }

  // figure out how many lines there are
  struct line_info linfo = count_lines(fptr);

  // rewind the file back to the beginning
  rewind(fptr);

  // make a 2d array of all the characters
  struct array2d array = file_to_array(fptr, linfo);

  // now solve it out
  int result = solve(array, 1);
  printf("result for problem 1: %d", result);

  result = solve(array, 2);
  printf("result for problem 2: %d", result);

}
