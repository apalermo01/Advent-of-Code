#include <stdio.h>
#include <stdlib.h>

#define BUF_SIZE 65536

struct line_info {
  int num_lines;
  int max_width;
};

/* 
 * adapted from: https://stackoverflow.com/a/70708991
 */
struct line_info count_lines(FILE *file) {
  char buf[BUF_SIZE];
  int counter = 0;
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
        counter++;
        if (width_counter > max_width) {
          max_width = width_counter;
        }
        width_counter = 0;
      } else {
        width_counter ++;
      }
    }

    if (feof(file)) {
      break;
    }
  }
  
  ret.num_lines = counter;
  ret.max_width = max_width;
  return ret;
}

/*
 * Given some buffer and starting at the position indicated by starting_pos,
 * iterate backwards through the buffer until we hit a non-number,
 * then concatenate the resulting characters to a number and return it.
 */
int backward_search(char * buf, int starting_pos) {
  return 0;
}

/*
 * Given some buffer and starting at the position indicated by starting_pos,
 * iterate forwards through the buffer until we hit a non-number,
 * then concatenate the resulting characters to a number and return it.
 */
int forward_search(char * buf, int starting_pos) {
  return 0;
}

/*
 * Given some buffer and starting at the position indicated by starting_pos,
 * iterate forwards through the buffer until we hit a non-number, then iterate 
 * backwards until we hit a non-number, then concatenate the resulting characters 
 * to a number and return it.
 */
int middle_search(char * buf, int starting_pos) {
  return 0;
}
/*
 * core logic for problem 1 
 * Objective: the 2d array has a collection of numbers and symbols
 * A part number is defined as any number adjacent to a symbol. (
 * A symbol is anything that is not a digit or '.'). We want to 
 * add up all the part numbers in the array
 */
int problem_1(char * array) { 

  // pad the array so we can treat everything equally

  return 0;
}

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
  
  // make a 2d array of all the characters
  char * array; // finish this out

  // now solve it out
  int result = problem_1(array);
}
