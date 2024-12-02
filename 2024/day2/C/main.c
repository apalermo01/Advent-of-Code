#include <ctype.h>
#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF_LENGTH 2048
#define MAX_NUM_LENGTH 64
#define NOT_SET 0
#define ASCENDING 1
#define DESCENDING 2 
#define FIRST_PART 1 
#define SECOND_PART 2

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

int check_line(int* nums, int len) {
  /*printf("nums = ");*/
  /*for (int i = 1; i < len; i++) {*/
  /*  printf("%d,", nums[i]);*/
  /*}*/
  /*printf("\n");*/
  int order = NOT_SET;
  for (int i = 1; i < len; i++) {
    if (order == NOT_SET && nums[i] > nums[i-1]) {
      order = ASCENDING;
    } 
    if (order == NOT_SET && nums[i] < nums[i-1]) {
      order = DESCENDING;
    } 
    
    if (
      (order == ASCENDING && nums[i] < nums[i-1]) ||
      (order == DESCENDING && nums[i] > nums[i-1]) ||
      (abs(nums[i] - nums[i-1]) > 3) ||
      (nums[i] == nums[i-1])
    ) {
      return 0;
    }

  }
  return 1;

}
int process_line_part1(char* line) {

  int len = strnlen(line, MAX_BUF_LENGTH);

  if (len == MAX_BUF_LENGTH) {
    fprintf(stderr, "error - maximum line length reached");
    return 0;
  }
  
  int* nums = malloc(len * sizeof(int));

  if (nums == NULL) {
    fprintf(stderr, "memory allocation failed");
    return 0;
  }

  int index = 0;
  char delim = ' ';
  char* line_copy = line;
  char* token;
  int num;
  while ((token = strtok(line_copy, &delim)) != NULL) {
    nums[index] = string_to_num(token);
    line_copy = NULL;
    index++;
  }

  if (index < 2) {
    free(nums);
    return 0;
  }
  
  int res = check_line(nums, index);
  free(nums);
  return res;
}

void build_new_array(int* nums2, const int* nums, const int len, const int index) {
  int j = 0;
  for (int i = 0; i < len; i++) {
    if (i == index) {
      continue;
    }

    nums2[j] = nums[i];
    j++;
  }
}

int process_line_part2(char* line) {

  int len = strnlen(line, MAX_BUF_LENGTH);

  if (len == MAX_BUF_LENGTH) {
    fprintf(stderr, "error - maximum line length reached");
    return 0;
  }
  
  int* nums = malloc(len * sizeof(int));
  int* nums2 = malloc((len-1) * sizeof(int));

  if (nums == NULL) {
    fprintf(stderr, "memory allocation failed");
    return 0;
  }

  if (nums2 == NULL) {
    fprintf(stderr, "memory allocation failed");
    return 0;
  }

  int index = 0;
  char delim = ' ';
  char* line_copy = line;
  char* token;
  int num;
  while ((token = strtok(line_copy, &delim)) != NULL) {
    nums[index] = string_to_num(token);
    line_copy = NULL;
    index++;
  }
  
  if (index < 2) {
    free(nums);
    free(nums2);
    return 0;
  }
  
  int result = 0;

  for (int i = 0; i < index; i++) {
    build_new_array(nums2, nums, index, i);
    /*printf("nums  = ");*/
    /*for (int j = 0; j < index; j++) {*/
    /*  printf("%d,", nums[j]);*/
    /*}*/
    /*printf("\n");*/
    /*printf("nums2 = ");*/
    /*for (int j = 0; j < index-1; j++) {*/
    /*  printf("%d,", nums2[j]);*/
    /*}*/
    /*printf("\n");*/
    result = check_line(nums2, index-1);

    if (result == 1) {
      /*printf("safe!\n");*/
      return result;
    }
  }

  return 0;
}

int main(int argc, char *argv[]) {
  if (argc != 3) {
    printf("usage: ./main input_file problem_section (1 or 2)\n");
    exit(1);
  }

  if (strnlen(argv[1], MAX_BUF_LENGTH) > MAX_BUF_LENGTH) {
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
    fprintf(stderr, "second argument must be 1 or 2\n");
    free(fptr);
    exit(1);
  }

  int mode = string_to_num(&mode_char);
  if ((mode != 1) && (mode != 2)) {
    fprintf(stderr, "second argument must be 1 or 2");
    free(fptr);
    exit(1);
  }

  int num_safe = 0;

  ssize_t read;
  size_t len = 0;
  char *line = NULL;
  int is_safe;
  while ((read = getline(&line, &len, fptr)) != -1) {
    if (mode == FIRST_PART) {
      is_safe = process_line_part1(line);
    } else {
      is_safe = process_line_part2(line);
    }
    num_safe += is_safe;
  }

  printf("num safe = %d\n", num_safe);

  free(line);
}
