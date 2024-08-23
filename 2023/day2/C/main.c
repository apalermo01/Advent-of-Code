#include <stdio.h>
#include <stdlib.h>
#include <regex.h>
#include <string.h>

int NUM_RED = 12;
int NUM_GREEN = 13;
int NUM_BLUE = 14;
int MAXLEN = 2048;
int get_id(char *line);

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
  
  printf("line = %s", line);
  
  // exit condition
  if ((num_red > NUM_RED) || (num_green > NUM_GREEN) || (num_blue > NUM_BLUE)) {
    return 0;
  }

  return id;
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

  comp_value = regcomp(&regex, "\d+(?=:)", 0);

  if (comp_value == 0) {
    printf("Regex  compiled successfully\n");
  } else {
    printf("compilation error");
    return -1;
  }
  
  match_value = regexec(&regex, line, 1, &regmatch, 0);

  if (match_value == 0) {
    printf("matching successful");
  } else {
    char *errormsg = malloc(1024* sizeof(int));
    regerror(match_value, &regex, errormsg, 1024);
    printf("matching error: %s", errormsg);
    free(errormsg);
  }
  
  regfree(&regex);
  // set up a char array to hold the characters
  // char *id_chr = malloc(4*sizeof *id_chr);
  //int game_id = 0;

  //// loop backwards from the :
  //for (int i = 5; i < 0; i--) {
  //  if (isdigit(line[i])) {
  //    if (game_id == )
  //  }
  //}

  return -1;
}


int main() {
  // declare some vars
  FILE *fptr;
  char text[255];
  int total = 0;
  /* part 1 */ 
  // Open the file
  if ((fptr = fopen("../sample_input_1.txt", "r")) == NULL) {
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
