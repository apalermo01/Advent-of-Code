#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))
#define MAX_BUF_LENGTH 2048
#define MAX_DIGITS 4

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

int process_line_v1(const char *line) {

  regex_t reg, reg_num;
  regmatch_t regmatch[3];
  size_t nmatch;
  regoff_t off, len;
  int err;
  int exec_result;
  int num1, num2;
  int total = 0;
  const char *s = line;
  const char *re = "mul\\(([0-9]+),([0-9]+)\\)";

  if ((err = regcomp(&reg, re, REG_EXTENDED))) {
    char *errbuff = malloc(2048 * sizeof(char));
    fprintf(stderr, "failed to compile regex\n");
    regerror(err, &reg, errbuff, 2048);
    fprintf(stderr, "%s\n", errbuff);
    free(errbuff);
    exit(EXIT_FAILURE);
  }

  for (int i = 0;; i++) {
    if ((exec_result = regexec(&reg, s, ARRAY_SIZE(regmatch), regmatch, 0))) {
      break;
    }

    num1 = get_num_from_regmatch(regmatch[1], s, line);
    num2 = get_num_from_regmatch(regmatch[2], s, line);
    total += (num1 * num2);
    s += regmatch[0].rm_eo;
  }
  regfree(&reg);
  return total;
}

int process_line_v2(const char *line, int* do_calc) {

  regex_t reg, reg_num;
  regmatch_t regmatch[3];
  size_t nmatch;
  regoff_t off, len;
  int err;
  int exec_result;
  int num1, num2;
  int total = 0;
  const char *s = line;
  const char *re = "mul\\(([0-9]+),([0-9]+)\\)|do\\(\\)|don't\\(\\)";

  /*printf("line = %s\n", line);*/

  if ((err = regcomp(&reg, re, REG_EXTENDED))) {
    char *errbuff = malloc(2048 * sizeof(char));
    fprintf(stderr, "failed to compile regex\n");
    regerror(err, &reg, errbuff, 2048);
    fprintf(stderr, "%s\n", errbuff);
    free(errbuff);
    exit(EXIT_FAILURE);
  }

  for (int i = 0;; i++) {
    if ((exec_result = regexec(&reg, s, ARRAY_SIZE(regmatch), regmatch, 0))) {
      break;
    }

    off = regmatch[0].rm_so + (s - line);
    len = regmatch[0].rm_eo - regmatch[0].rm_so;

    printf("substring = %.*s\n", len, s + regmatch[0].rm_so);
    if (line[off] == 'd' && line[off + 2] == 'n') {
      printf("line @ off+2 = %c\n", line[off+2]);
      *do_calc = 0;
    } else if (line[off] == 'd' && line[off + 2] != 'n') {
      printf("line @ off+2 = %c\n", line[off+2]);
      *do_calc = 1;
    } else if (*do_calc == 1) {
      num1 = get_num_from_regmatch(regmatch[1], s, line);
      num2 = get_num_from_regmatch(regmatch[2], s, line);
      total += (num1 * num2);
      printf("doing calculation: %d\n", num1 * num2);
    }
    /*printf("num1 = %d\n", num1);*/
    /*printf("num2 = %d\n", num2);*/

    s += regmatch[0].rm_eo;
  }
  regfree(&reg);
  return total;
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
  int do_calc = 1;
  while ((read = getline(&line, &len, fptr)) != -1) {
    if (mode == 1) {
      total += process_line_v1(line);
    }
    if (mode == 2) {
      total += process_line_v2(line, &do_calc);
    }
  }

  printf("total is %d\n", total);
}
