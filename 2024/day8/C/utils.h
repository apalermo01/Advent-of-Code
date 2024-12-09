
#include <regex.h>
#include <stdio.h>

struct array2d {
  char *arr;
  int numrows;
  int numcols;
};

struct line_info {
  int num_lines;
  int max_width;
};

int string_to_num(const char *str);

int get_num_from_regmatch(const regmatch_t regmatch, const char *s,
                          const char *line);

struct line_info count_lines(FILE *file);

struct array2d make_array(int nrows, int ncols);

int set_in_array(struct array2d arr, int row, int col, char value);

int get_value(struct array2d arr, int row, int col, int p);

void freearr(struct array2d *arr);

void print_row(struct array2d array, int row);

void get_row(struct array2d array, int row, char *buf, size_t buffsize);

struct array2d file_to_array(FILE *file, struct line_info linfo);

void get_substring(char *dest, const struct array2d arr, int row, int col,
                   int num_chars);
