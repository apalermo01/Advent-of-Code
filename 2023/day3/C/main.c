#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 65536

// define a structure for returning a tuple that represents the information we
// need to grab from the file before doing anything else
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

  printf("coutner = %d\n", row_counter);
  printf("max width = %d\n", max_width);
  ret.num_lines = row_counter;
  ret.max_width = max_width;
  return ret;
}

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

void freearr(struct array2d *arr) {
  free(arr->arr);
  free(arr);
}

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
    /*printf("line = %s\n", line);*/
    for (thiscol = 0; thiscol < ncols; thiscol++) {
      /*printf("setting (%d, %d) to %c\n", thisrow, thiscol, line[thiscol]);*/
      set_in_array(arr, thisrow, thiscol, line[thiscol]);
    }
    thisrow++;
  }

  return arr;
}

void print_row(struct array2d array, int row) {
  if (row >= array.numrows) {
    fprintf(stderr, "ERROR: row %d is out of bounds for array with %d rows\n", row, array.numrows);
  }
  for (int j = 0; j < array.numcols; j++) {
    printf("%c", get_value(array, row, j));
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
  for (int i = 0; i < nrows-1; i++) {

    if (set_in_array(new_array, i, 0, '.') != 0) {
      fprintf(stderr, "error assigning value: row=%d, col=%d\n", i, 0);
    }
    if (set_in_array(new_array, i, ncols-1, '.') != 0) {
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
int backward_search(char *buf, int starting_pos) { return 0; }

/*
 * Given some buffer and starting at the position indicated by starting_pos,
 * iterate forwards through the buffer until we hit a non-number,
 * then concatenate the resulting characters to a number and return it.
 */
int forward_search(char *buf, int starting_pos) { return 0; }

/*
 * Given some buffer and starting at the position indicated by starting_pos,
 * iterate forwards through the buffer until we hit a non-number, then iterate
 * backwards until we hit a non-number, then concatenate the resulting
 * characters to a number and return it.
 */
int middle_search(char *buf, int starting_pos) { return 0; }
/*
 * core logic for problem 1
 * Objective: the 2d array has a collection of numbers and symbols
 * A part number is defined as any number adjacent to a symbol. (
 * A symbol is anything that is not a digit or '.'). We want to
 * add up all the part numbers in the array
 */

int problem_1(struct array2d text) {

  for (int i = 0; i < text.numrows; i++) {
    for (int j = 0; j < text.numcols; j++) {
    }
  }

  // pad the array so we can treat everything equally
  text = pad_array(text);

  for (int i = 0; i < text.numrows; i++) {
    print_row(text, i);
    printf("\n");
  }
  return 0;
}

// test the 2d array functionality
/*
int main() {
  int numrows = 2;
  int numcols = 3;

  struct array2d my_array = make_array(numrows, numcols);

  for (int i = 0; i < numrows; i++) {
    for (int j = 0; j < numcols; j++) {
      set_in_array(my_array, i, j, (char) (i*j));
    }
  }
}
*/

int main() {
  FILE *fptr;

  if ((fptr = fopen("../sample_input_1.txt", "r")) == NULL) {
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
  int result = problem_1(array);
}
