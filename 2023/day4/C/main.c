#include <stdio.h>
#include <stdlib.h>
#include <string.h>

size_t MAX_BUF_LENGTH = 2048;

int process_line(char *line) {
  return 0;
}

int calculate_score(FILE *file) {
  int score = 0;
  size_t len = 0;
  ssize_t read;
  char *line = NULL;

  while  ((read = getline(&line, &len, file)) != -1) {
    printf("line = %s", line);
  }

  return score;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("exactly one argument (input name) is required!");
    exit(1);
  }
  
  if (strnlen(argv[1], MAX_BUF_LENGTH) > MAX_BUF_LENGTH) {
    printf("input exceeds max length of %zu", MAX_BUF_LENGTH);
    exit(1);
  }

  
  FILE *fptr;

  printf("opening file %s", argv[1]);
  if ((fptr = fopen(argv[1], "r")) == NULL) {
    fprintf(stderr, "Error opening file");
    free(fptr);
    exit(1);
  }

  int score = calculate_score(fptr);
  printf("score is %d", score);

}
