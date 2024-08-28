#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>
#include <ctype.h>

/* TODO: update regex matching to reliable extract multidigit numbers
 */

int NUM_RED = 12;
int NUM_GREEN = 13;
int NUM_BLUE = 14;
int MAXLEN = 2048;
int get_id(char *line);
int get_color_counts(char *line, char *color);

unsigned concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while (y >= pow) {
        pow *= 10;
    }
    return x * pow + y;
}

int extract_number(char *line, int se, int so) {
  return 0;
}
/* 
 * Determine whether or not a given game is possible
 * a line is structured like this: 
 * Game <id>: # color1, # color2, # color3; # color1, ...
 * Return the id of the game if it is possible,
 * return 0 otherwise
 */
int process_line(char *line) {
  int id = get_id(line);        // counter
  int num_red = 0;    // number of observed colors
  int num_green = 0;
  int num_blue = 0;
  
  num_red += get_color_counts(line, "red");
  num_green += get_color_counts(line, "green");
  num_blue += get_color_counts(line, "blue");
  
  printf("==============\n");
  printf("line = %s", line);
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

  size_t max_matches = 32;

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
    if (regexec(&regex, cursor, 1, &regmatch, 0)) {
      break; // no more matches
    }

    unsigned int g = 0;
    unsigned int match_start = 0;
    unsigned int match_end = 0;

    match_start = regmatch.rm_so;           /* byte offset from start of string to 
                                               start of substring */
    match_end = regmatch.rm_eo;                /* bye offset from start of string to 
                                               the first character after the end of 
                                               the substring*/
    // concat if there is a tens digit
    match_value = cursor[match_start];
    if (isdigit(cursor[match_start+1])) {
      int singles = cursor[match_start+1] - '0';
      int tens = match_value - '0';
      match_value = concatenate(tens, singles);
      total += match_value;
    } else {
      total += match_value - '0';
    }
    cursor += match_end;
  }

  free(expr);
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
  regmatch_t regmatch;

  int comp_value;
  int match_value;

  comp_value = regcomp(&regex, "([[:digit:]]+):", REG_EXTENDED);
  
  if (comp_value != 0) {
    fprintf(stderr, "regex compilation error");
    return -1;
  }
  
  match_value = regexec(&regex, line, 1, &regmatch, 0);

  if (match_value != 0) {
    char *errormsg = malloc(1024* sizeof(int));
    regerror(match_value, &regex, errormsg, 1024);
    printf("matching error: %s", errormsg);
    free(errormsg);
    return -1;
  }
  
  regfree(&regex);
  
  // now get the actual match
  // we're only looking at matching a single character,
  // so use the index rm_so
  int rm_so = regmatch.rm_so;
  int game_id = line[rm_so] - '0';
  printf("game id = %d\n", game_id); 
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
