#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>

int NUM_RED = 12;
int NUM_GREEN = 13;
int NUM_BLUE = 14;
int MAXLEN = 2048;
int get_id(char *line);
int get_color_counts(char *line, char *color);

/*
 * TODO: need to clean this up to handle more edge cases
 * y = 0, x = 0 -> 100
 */
unsigned concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while (y >= pow) {
        pow *= 10;
    }
    if (y == 0) {
        pow *= 10;
    }
    return x * pow + y;
}

struct regex_num {
  int value;
  int offset;
};
/*
 * Runs a regex (pattern) on an input string with potentially multiple matches.
 * returns the matched integer. This is built to handle up to max_num characters
 */
struct regex_num number_from_regex(char *line,
                                   regex_t *pattern,
                                   int max_digits) {
  struct regex_num result;
  result.value = 0;
  result.offset = 0;

  regmatch_t regmatch;
  char * digitbuff = malloc(max_digits * sizeof(char));

  if (digitbuff == NULL) {
    fprintf(stderr, "memory allocation failed!");
    return result;
  }

  if (regexec(pattern, line, 1, &regmatch, 0)) {
    // no matches
    return result;
  }
  
  unsigned int match_start = regmatch.rm_so;
  unsigned int match_end = regmatch.rm_eo;

  int num_digits = 0;

  while (isdigit(line[match_start+num_digits]) && (num_digits < max_digits)) {
    digitbuff[num_digits] = line[match_start + num_digits];
    num_digits ++;
  }
  printf("digitbuff = %s\n", digitbuff); 
  int digit_position = num_digits-1;
  int total = digitbuff[digit_position] - '0';
  printf("digit position = %d, digitbuff@position = %c\n", digit_position, digitbuff[digit_position]);
  while (digit_position > 0) {
    digit_position --;
    printf("digit position = %d, digitbuff@position = %c\n", digit_position, digitbuff[digit_position]);
    total = concatenate(digitbuff[digit_position] - '0', total);
    printf("total = %d\n", total);
  }

  result.value = total;
  result.offset = match_end;
  free(digitbuff);
  return result;
}

/* 
 * Determine whether or not a given game is possible
 * a line is structured like this: 
 * Game <id>: # color1, # color2, # color3; # color1, ...
 * Return the id of the game if it is possible,
 * return 0 otherwise
 */
int process_line(char *line) {
  printf("==============\n");
  printf("line = %s\n", line);

  int id = get_id(line);
  printf("id = %d\n", id); 
  int num_red = get_color_counts(line, "red");
  int num_green = get_color_counts(line, "green");
  int num_blue = get_color_counts(line, "blue");
  
  printf("red = %d; green = %d; blue = %d\n", num_red, num_green, num_blue);

  // check if game is impossible
  if ((num_red > NUM_RED) || (num_green > NUM_GREEN) || (num_blue > NUM_BLUE)) {
    printf("this game is impossible\n\n");
    return 0;
  }
  
  printf("this game is possible\n\n");
  return id;
}

/* 
 * For an arbitrary line and color, return the number of times that color
 * is encountered. This will look for the pattern `# color`, which may
 * occur an arbitrary number of times, and return the #. Returns 0
 * in case of a failure
 */
int get_color_counts(char *line, char *color) {

  // initialize some variables
  regex_t regex;
  regmatch_t regmatch;

  size_t max_matches = 2;

  char match_value;
  int total = 0;

  // validate inputs
  if (line == NULL || color == NULL) {
    fprintf(stderr, "line and color are null pointers");
    return 0;
  }

  /*
   * construct the regex string
   * string format is "[[:digit:]]+(?= %s)" where %s is the string for pos lookahead
   */
  int colorlen = strnlen(color, 128);
  char *start_of_regex = "([[:digit:]]+)";
  if (colorlen > 126) { // don't remember if it's inclusive or exclusive
    fprintf(stderr, "too many characters for color. 125 max");
    return 0;
  }

  size_t regexlen = strlen(start_of_regex) + colorlen + 2;
  char *expr = malloc(regexlen);
  
  // allocation failure
  if (expr == NULL) {
    fprintf(stderr, "error allocating memory for regex expression");
    return 0;
  }

  // build concatenated string
  sprintf(expr, "([[:digit:]]+) %s", color);

  /* compile the regex */
  if(regcomp(&regex, expr, REG_EXTENDED)) {
    fprintf(stderr, "regex compilation error\n");
    free(expr);
    return -1;
  }

  /* 
   * Parse matches
   * referencing https://gist.github.com/ianmackinnon/3294587
   * to get multiple matches
   */

  /* set cursor = line. Every time a match is found,
   * cursor is a pointer to the first element of a char array representing the line
   * every time we find a match, we'll increase cursor by offset,
   * effectively moving it forward so the match isn't repeated
   */
  char * cursor;
  cursor = line;
  unsigned int m;

  for (m = 0; m < max_matches; m++) {
    struct regex_num match_info = number_from_regex(cursor, &regex, 3); 
    total += match_info.value;
    cursor += match_info.offset;
  }

  free(expr);
  regfree(&regex); 
  return total;
}

/*
 * Function to get the id of a Game
 */
int get_id(char *line) {

  // check that there are enough characters
  if (strnlen(line, 2048) < 6) {
    fprintf(stderr, "invalid line");
    return -1;
  }

  regex_t regex;

  int comp_value;
  int match_value;

  comp_value = regcomp(&regex, "([[:digit:]]+):", REG_EXTENDED);
  
  if (comp_value != 0) {
    fprintf(stderr, "regex compilation error");
    return -1;
  }

  struct regex_num match_info = number_from_regex(line, &regex, 3);

  int game_id = match_info.value;

  return game_id;
}


int main() {
  // declare some vars
  FILE *fptr;
  char text[255];
  int total = 0;

  /* part 1 */ 
  // Open the file
  //if ((fptr = fopen("../sample_input_1.txt", "r")) == NULL) {
  if ((fptr = fopen("../input_1.txt", "r")) == NULL) {
    fprintf(stderr, "Error opening file");
    free(fptr);
    exit(1);
  }

  printf("File opening successful\n");

  while (fgets(text, sizeof(text), fptr)) {
    int num_this_line = process_line(text);
    total += num_this_line;
  }
  fclose(fptr);
  printf("total part 1: %d\n", total);
  return 0;
}
