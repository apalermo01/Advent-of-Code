#include <ctype.h>
#include <errno.h>
#include <regex.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX_BUF_LENGTH 2147483
#define ARRAY_SIZE(arr) (sizeof((arr)) / sizeof((arr)[0]))

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

struct page_rules {
  int first_page;
  int second_page;
};

struct update {
  int page_nums[256];
  int middle_index;
  int num_pages;
};

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

struct page_rules get_page_rule(char* line) {
  struct page_rules rule;
  
  regex_t reg, reg_num;
  regmatch_t regmatch[3];
  regoff_t off, len;
  int err, exec_result;
  int num1, num2;
  const char *s = line;
  const char *re = "([0-9]+)\\|([0-9]+)";
  if ((err = regcomp(&reg, re, REG_EXTENDED))) {
    char *errbuff = malloc(2048 * sizeof(char));
    fprintf(stderr, "failed to compile regex\n");
    regerror(err, &reg, errbuff, 2048);
    fprintf(stderr, "%s\n", errbuff);
    free(errbuff);
    exit(EXIT_FAILURE);
  }

  if ((exec_result = regexec(&reg, s, ARRAY_SIZE(regmatch), regmatch, 0))) {
    return rule;
  }
  rule.first_page = get_num_from_regmatch(regmatch[1], s, line);
  rule.second_page = get_num_from_regmatch(regmatch[2], s, line);
  
  regfree(&reg);
  return rule;
}

struct update get_update(char* line) {
  struct update updates;
  
  regex_t reg, reg_num;
  regmatch_t regmatch[2];
  regoff_t off, len;
  int err, exec_result;
  int num1, num2;
  const char *s = line;
  const char *re = "([0-9]+)";
  if ((err = regcomp(&reg, re, REG_EXTENDED))) {
    char *errbuff = malloc(2048 * sizeof(char));
    fprintf(stderr, "failed to compile regex\n");
    regerror(err, &reg, errbuff, 2048);
    fprintf(stderr, "%s\n", errbuff);
    free(errbuff);
    exit(EXIT_FAILURE);
  }
  
  int num_pages = 0;
  for (int i = 0;; i++) {
    if ((exec_result = regexec(&reg, s, ARRAY_SIZE(regmatch), regmatch, 0))) {
      break;
    }

    num1 = get_num_from_regmatch(regmatch[1], s, line);
    updates.page_nums[i] = num1;
    s += regmatch[0].rm_eo;
    num_pages = i;
  }
  regfree(&reg);

  updates.middle_index = num_pages / 2;
  updates.num_pages = num_pages + 1;
  return updates;
}

int check_update(struct update updates, struct page_rules* rules, int num_rules) {
  int pg1, pg2;
  for (int i = 0; i < num_rules; i++) {
    pg1 = rules[i].first_page;
    pg2 = rules[i].second_page;
    
    int first_page_present = 0;
    int first_page_updated = 0;

    for (int j = 0; j < updates.num_pages; j++) {
      if(updates.page_nums[j] == pg1) {
        first_page_present = 1;
      }
    }

    for (int j = 0; j < updates.num_pages; j++) {
      if(updates.page_nums[j] == pg1) {
        first_page_updated = 1;
      }

      if ((updates.page_nums[j] == pg2) &&
          (first_page_present == 1) && 
          (first_page_updated == 0)) {
        return 0;
      }

    }

  }
  return 1;
}

void find_and_do_switch(struct update* updates, struct page_rules* rules, int num_rules) {
  int pg1, pg2;
  for (int i = 0; i < num_rules; i++) {
    pg1 = rules[i].first_page;
    pg2 = rules[i].second_page;
    
    int first_page_present = 0;
    int first_page_updated = 0;
    int first_page_index = 0;

    for (int j = 0; j < updates->num_pages; j++) {
      if(updates->page_nums[j] == pg1) {
        first_page_present = 1;
        first_page_index = j;
      }
    }

    for (int j = 0; j < updates->num_pages; j++) {
      if(updates->page_nums[j] == pg1) {
        first_page_updated = 1;
      }

      if ((updates->page_nums[j] == pg2) &&
          (first_page_present == 1) && 
          (first_page_updated == 0)) {
        int tmp = updates->page_nums[first_page_index];
        updates->page_nums[first_page_index] = updates->page_nums[j];
        updates->page_nums[j] = tmp;
        return;
      }
    }
  }
}

int fix_update(struct update updates, struct page_rules* rules, int num_rules) {
  
  int is_correct = 0;
  int max_iter = 2048;
  int current_iter = 0;
  while (is_correct == 0) {
    if (current_iter > max_iter) {
      fprintf(stderr, "maximum iterations reached");
      break;
    } 

    find_and_do_switch(&updates, rules, num_rules);
    if (check_update(updates, rules, num_rules) == 1) {
      break;
    }
    current_iter ++;
  }
  
  return updates.page_nums[updates.middle_index];
}

int solve_v1(FILE *fptr) {
  printf("running solve v1\n");
  int result = 0;
  int max_rules = 2048;
  int max_updates = 2048;
  
  struct page_rules* rules = malloc(max_rules * sizeof(struct page_rules));
  struct update* updates = malloc(max_updates * sizeof(struct update));

  if (rules == NULL || updates == NULL) {
    fprintf(stderr, "failed to allocate memory");
    return 0;
  }

  ssize_t read;
  size_t len = 0;
  char *line = NULL;
  
  int mode = 0; // 0 for reading rules, 1 for reading updates
  int rule_num = 0;
  int update_num = 0;
  while ((read = getline(&line, &len, fptr)) != -1) {
    
    if(strnlen(line, 1024) < 2) {
      mode = 1;
    }

    if(mode == 0) {
      if(rule_num > max_rules) {
        printf("max rules eached");
        continue;
      }
      rules[rule_num] = get_page_rule(line); 
      rule_num ++;
    }

    if(mode == 1) {
      if(update_num > max_updates) {
        printf("max updates reached");
        continue;
      }

      if(strnlen(line, 2048) < 2) {
        continue;
      }
      struct update u = get_update(line);
      if(u.num_pages > 0) {
        updates[update_num] = u;
        update_num ++;
      }
    }
  }
  
  int total = 0;
  int total2 = 0;
  for (int i = 0; i < update_num; i++) {
    if(check_update(updates[i], rules, rule_num) == 1) {
      total += updates[i].page_nums[updates[i].middle_index];
    } else {
      total2 += fix_update(updates[i], rules, rule_num);
    }

  }
  free(rules);
  free(updates);
  printf("total = %d\n", total);
  printf("total for fixes = %d\n", total2);
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


  // figure out how many lines there are
  /*struct line_info linfo = count_lines(fptr);*/

  // rewind the file back to the beginning
  /*rewind(fptr);*/

  // make a 2d array of all the characters
  /*struct array2d array = file_to_array(fptr, linfo);*/
  if (mode == 1) {
    total = solve_v1(fptr);
  }

  /*if (mode == 2) {*/
  /*  total = solve_v2(array);*/
  /*}*/
  /*printf("total is %d\n", total);*/
}
