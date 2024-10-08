#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>

// constants
int NUM_RED = 12;
int NUM_GREEN = 13;
int NUM_BLUE = 14;
int MAXLEN = 2048;

// types and prototypes
struct regex_num {
  int value;
  int offset;
};

int get_id(char *line);
int get_color_counts(char *line, char *color);
void advance_to_next_round(char *line, char **dest);
int string_to_num(char *str);
struct regex_num number_from_reges(char *line, regex_t *pattern, int max_digits);
int process_line(char *line);
int process_line_version2(char *line);

int string_to_num(char *str) {
  int total = 0;
  int pow = 1;

  int num_digits = strnlen(str, 8);
  int num;

  for(int i = num_digits; i >= 0; i--){
    if (str[i] == '\0') continue;
    num = str[i] - '0';
    total += pow * num;
    pow *= 10;
  }

  return total;
}

/*
 * Runs a regex (pattern) on an input string with potentially multiple matches.
 * returns the matched integer. This is built to handle up to max_num characters
 */
struct regex_num number_from_regex(char *line,
                                   regex_t *pattern,
                                   int max_digits) {

  // init some variables
  struct regex_num result;
  result.value = 0;
  result.offset = 0;

  regmatch_t regmatch;

  /* 
  printf("total part 1: %d\n", total_part1);
   * digitbuff is a char buffer that holds a string representation of the 
   * integers for a specific mathc. For example,
   * if line is "abc1def", then digitbuff is "1"
   * if line is "abc123def", then digitbuff is "123".
   * The purpose of this is to isolate the digits in the match
   * so that the concat function can take over and get the actual number
   */
  char * digitbuff = malloc(max_digits * sizeof(char));

  if (digitbuff == NULL) {
    fprintf(stderr, "memory allocation failed!");
    return result;
  }
  
  // no matches
  if (regexec(pattern, line, 1, &regmatch, 0)) {
    return result;
  }
 
  // parse the matches
  unsigned int match_start = regmatch.rm_so;
  unsigned int match_end = regmatch.rm_eo;

  int num_digits = 0;
  
  while (isdigit(line[match_start+num_digits]) && (num_digits < max_digits)) {
    digitbuff[num_digits] = line[match_start + num_digits];
    num_digits ++;
  }

  int i = num_digits;
  while (i <= max_digits) {
    digitbuff[i] = '\0';
    i ++;
  }
  
  // do the concatenation and return the results
  int total = string_to_num(digitbuff);

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

  int id = get_id(line);
  int num_red = 0;
  int num_blue = 0;
  int num_green = 0;
  const size_t buffer_size = 1024; 

  char *cursor = line;

  while(cursor != NULL) {
    num_red = get_color_counts(cursor, "red");
    num_green = get_color_counts(cursor, "green");
    num_blue = get_color_counts(cursor, "blue");
    
    // check if game is impossible
    if ((num_red > NUM_RED) || (num_green > NUM_GREEN) || (num_blue > NUM_BLUE)) {
      return 0;
    }

    // advance line to after the next ';'

    advance_to_next_round(cursor, &cursor);
  } 
 
  return id;
}

/*
 * For each set of games listed out in line, determine the fewest number of cubes
 * needed to make the set of games possible. In other words, find the maximum value
 * of red, green, and blue from each set. Return the Power set of these - aka
 * their product
 */
int process_line_version2(char *line) {

  int max_red = 0;
  int num_red_this_game = 0;
  int max_blue = 0;
  int num_blue_this_game = 0;
  int max_green = 0;
  int num_green_this_game = 0;
  int prod = 1;

  const size_t buffer_size = 1024; 

  char *cursor = line;

  while(cursor != NULL) {
    num_red_this_game = get_color_counts(cursor, "red");
    num_green_this_game = get_color_counts(cursor, "green");
    num_blue_this_game = get_color_counts(cursor, "blue");

    if (num_red_this_game > max_red) max_red = num_red_this_game;
    if (num_blue_this_game > max_blue) max_blue = num_blue_this_game;
    if (num_green_this_game > max_green) max_green = num_green_this_game;

    // advance line to after the next ';'

    advance_to_next_round(cursor, &cursor);
  } 

  prod = max_red * max_blue * max_green;

  return prod;
 
}

 /* Advance the pointer to the character after the next ';'
  * If there is no ;, return a null pointer.
  */
void advance_to_next_round(char *line, char **dest) {

  if (line == NULL || dest == NULL) {
    fprintf(stderr, "advance_to_next_round: got null pointers for line and dest\n");
    dest = NULL;
    return;
  }
  
  regex_t regex;
  regmatch_t regmatch;
  char *expr = ";";
  
  if(regcomp(&regex, expr, REG_EXTENDED) != 0) {
    // regex didn't compile
    fprintf(stderr, "advance_to_next_round: regex compilation error\n");
    dest = NULL;
    return;
  }

  if(regexec(&regex, line, 1, &regmatch, 0) != 0) {
    // no more matches
    *dest = NULL;   

  } else {
    *dest = (char *)(line + regmatch.rm_eo);
  }
  regfree(&regex);
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

  char match_value;
  int total = 0;
  
  // validate inputs
  if (line == NULL || color == NULL) {
    fprintf(stderr, "line and color are null pointers\n");
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
    return 0;
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
  struct regex_num match_info = number_from_regex(cursor, &regex, 3);

  free(expr);
  regfree(&regex); 
  return match_info.value;
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
  int total_part1 = 0;
  int total_part2 = 0;

  int total_this_line_part1;
  int total_this_line_part2;

  /* part 1 */ 
  // Open the file
  if ((fptr = fopen("../input_1.txt", "r")) == NULL) {
    fprintf(stderr, "Error opening file");
    free(fptr);
    exit(1);
  }

  printf("File opening successful\n");

  while (fgets(text, sizeof(text), fptr)) {
    total_this_line_part1 = process_line(text);
    total_this_line_part2 = process_line_version2(text);
    total_part1 += total_this_line_part1;
    total_part2 += total_this_line_part2;
  }
  fclose(fptr);
  printf("total part 1: %d\n", total_part1);
  printf("total part 2: %d\n", total_part2);
  return 0;
}
