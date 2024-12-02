#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define FIRST_ELEMENT 1
#define SECOND_ELEMENT 2
#define MAX_BUF_LENGTH 2048
#define MAX_NUM_LENGTH 64
#define MAX_REPETITIONS 1000

struct nums_in_line {
  int num1;
  int num2;
};

struct list_ptrs {
  int* list1;
  int* list2;
  int count;
};

int string_to_num(char *str) {
  int total = 0;
  int pow = 1;

  int num_digits = strnlen(str, MAX_NUM_LENGTH);
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

int* bubble_sort(int* array, int length) {
  int made_swap = 0;
  int tmp;
  int count = 0;
  /*printf("running selection sort\n array = \n");*/
  /*for (int i = 0; i < length; i++) {*/
  /*  printf("%d\n", array[i]);*/
  /*}*/

  do {
    made_swap = 0;
    count ++;
    for (int i = 0; i < length; i++) {
      if (i > 0 && array[i] < array[i-1]) {
        tmp = array[i-1];
        array[i-1] = array[i];
        array[i] = tmp;
        made_swap = 1;
      }

    }
  } while (made_swap == 1 && count < MAX_REPETITIONS);

  /*printf("done with selection sort\n array = \n");*/
  /*for (int i = 0; i < length; i++) {*/
    /*printf("%d\n", array[i]);*/
  /*}*/

  return array;
}

struct nums_in_line process_line(char *line) {
  // define buffers to store the numbers
  char *number_str1 = malloc(MAX_NUM_LENGTH * sizeof(char));
  int num1_counter = 0;

  char *number_str2 = malloc(MAX_NUM_LENGTH * sizeof(char));
  int num2_counter = 0;

  // initially fill all these buffers with nulls
  for (int i = 0; i < MAX_NUM_LENGTH; i++) {
    number_str1[i] = '\0';
    number_str2[i] = '\0';
  }

  struct nums_in_line result = {
      .num1 = 0,
      .num2 = 0,
  };

  if (number_str1 == NULL || number_str2 == NULL) {
    fprintf(stderr, "Failed to allocate memory");
    return result;
  }
  int number_to_write = 1;

  for (int i = 0; i < MAX_NUM_LENGTH; i++) {
    if (line[i] == '\0') {
      break;
    }

    if (!isdigit(line[i])) {
      number_to_write = 2;
      continue;
    }

    if (number_to_write == 1) {
      number_str1[num1_counter] = line[i];
      num1_counter++;
    } else {
      number_str2[num2_counter] = line[i];
      num2_counter++;
    }
  }
  
  result.num1 = string_to_num(number_str1);
  result.num2 = string_to_num(number_str2);
  
  free(number_str1);
  free(number_str2);
  return result;
}

struct list_ptrs get_lists(FILE *file, struct list_ptrs ptrs) {
  int diffs = 0;
  size_t len = 0;
  ssize_t read;
  char *line = NULL;
  size_t num_lines = 0;

  while ((read = getline(&line, &len, file)) != -1) {
    num_lines++;
  }

  rewind(file);
  
  int *list1 = malloc(num_lines * sizeof(int));
  int *list2 = malloc(num_lines * sizeof(int));

  if (list1 == NULL || list2 == NULL) {
    fprintf(stderr, "memory allocation failed!");
    return ptrs;
  }
  
  struct nums_in_line nums;
  int counter = 0;

  while ((read = getline(&line, &len, file)) != -1) {
    nums = process_line(line);
    list1[counter] = nums.num1;
    list2[counter] = nums.num2;
    counter++;
  }
  
  ptrs.count = counter;
  ptrs.list1 = list1;
  ptrs.list2 = list2;
  return ptrs;
}
  
int calculate_diffs(struct list_ptrs ptrs) {
  int* list1 = ptrs.list1;
  int* list2 = ptrs.list2;
  int counter = ptrs.count;
  int diffs = 0;
  list1 = bubble_sort(list1, counter);
  list2 = bubble_sort(list2, counter);

  for (int i = 0; i < counter; i++) {
    if(list1[i] > list2[i]) {
      diffs += list1[i] - list2[i];
    } else if (list2[i] > list1[i]) {
      diffs += list2[i] - list1[i];
    }
  }
  return diffs;
}

int count_occurences(int element, int* list, int numel) {
  int occur = 0;
  for (int i = 0; i < numel; i++) {
    if (list[i] == element) {
      occur ++;
    }
  } 

  return occur;
}

int calculate_sim_score(struct list_ptrs ptrs) {
  
  int* list1 = ptrs.list1;
  int* list2 = ptrs.list2;
  int counter = ptrs.count;
  int sim_score = 0;

  for (int i = 0; i < counter; i++) {
    sim_score += list1[i] * count_occurences(list1[i], list2, counter);
  }

  return sim_score;
}

int main(int argc, char *argv[]) {
  if (argc != 2) {
    printf("exactly one argument (input name) is required!");
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
    free(fptr);
    exit(1);
  }
  
  struct list_ptrs ptrs;
  ptrs = get_lists(fptr, ptrs);

  int diffs = calculate_diffs(ptrs);
  printf("diffs is %d\n", diffs);
  
  int sim_score = calculate_sim_score(ptrs);
  printf("sim score is %d\n", sim_score);

  free (ptrs.list1);
  free (ptrs.list2);
}
