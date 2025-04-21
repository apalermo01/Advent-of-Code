#include "./utils.c"
#include <ctype.h>
#include <string.h>

#define UP 0
#define DOWN 1 
#define LEFT 2 
#define RIGHT 3

struct guard {
  int position_r;
  int position_c;
  int direction;
};


struct guard init_guard(struct array2d* array) {
  struct guard g;
  char val; 
  int guard_initialized = 0;
  for(int r = 0; r < array->numrows; r++) {
    for(int c = 0; c < array->numrows; c++) {
      val = get_value(*array, r, c, 0);
      if (val == '>' || val == '<' || val == '^' || val == 'V') {
        g.position_r = r;
        g.position_c = c;

        if (val == '>') {g.direction = RIGHT;}
        if (val == '<') {g.direction = LEFT;}
        if (val == '^') {g.direction = UP;}
        if (val == 'V') {g.direction = DOWN;}
        guard_initialized = 1;
        break;
      }
    }
  }

  if(guard_initialized == 0) {
    fprintf(stderr, "Could not initialize guard!\n");
  }

  return g;
}

struct array2d init_visited_array(int nrows, int ncols) {
  struct array2d arr = make_array(nrows, ncols);

  for (int r = 0; r < nrows; r++) {
    for (int c = 0; c < ncols; c++) {
      set_in_array(arr, r, c, 0);
    }
  }

  return arr;
}

int is_clear(struct guard* g, struct array2d* map) {
  /*printf("g direction = %d\n ", g->direction);*/
  if ((g->direction == UP) &&
      (get_value(*map, g->position_r-1, g->position_c, 0) == '.')) {
    return 1;
  }
  
  if ((g->direction == DOWN) &&
      (get_value(*map, g->position_r+1, g->position_c, 0) == '.')) {
    return 1;
  }
  if ((g->direction == LEFT) &&
      (get_value(*map, g->position_r, g->position_c-1, 0) == '.')) {
    return 1; 
  }

  if ((g->direction == RIGHT) && 
      (get_value(*map, g->position_r, g->position_c+1, 0) == '.')) {
    return 1;
  }

  /*fprintf(stderr, "error running is_clear\n");*/
  return 0;

}
void move_guard(struct guard* g, struct array2d* map) {
  if(is_clear(g, map) == 1) {
    if (g->direction == UP) {
      g->position_r --;
      return;
    }
    if (g->direction == DOWN) {
      g->position_r ++;
      return;
    }
    if (g->direction == LEFT) {
      g->position_c --;
      return;
    }
    if (g->direction == RIGHT) {
      g->position_c ++;
      return;
    }
  } else {
    if (g->direction == UP) {
      g->direction = RIGHT;
      return;
    }
    if (g->direction == RIGHT) {
      g->direction = DOWN;
      return;
    }
    if (g->direction == DOWN) {
      g->direction = LEFT;
      return;
    }
    if (g->direction == LEFT) {
      g->direction = UP;
      return;
    }
  }

}
int solve_v1(struct array2d array) {
  struct guard g = init_guard(&array);
  struct array2d visited_array = init_visited_array(array.numrows, array.numcols);

  set_in_array(visited_array, g.position_r, g.position_c, 1);
  set_in_array(array, g.position_r, g.position_c, '.');
  int max_iter = 10000;
  int count = 0;
  while (g.position_r > 0 &&
         g.position_r < array.numrows &&
         g.position_c > 0 &&
         g.position_c < array.numcols) {
    
    move_guard(&g, &array);
    set_in_array(visited_array, g.position_r, g.position_c, 1);
    /*printf("move guard to %d, %d\n", g.position_r, g.position_c);*/
    if (count > max_iter) {
      fprintf(stderr, "max iter reached\n");
      return 0;
    }

    count ++;
  }
  
  int distinct_visited = 0;
  for (int r = 0; r < visited_array.numrows; r++) {
    for (int c = 0; c < visited_array.numcols; c++) {
      distinct_visited += get_value(visited_array, r, c, 0);
    }
  }

  printf("visited %d distinct spots", distinct_visited);
  return distinct_visited;
}

int check_for_loop(struct array2d* array,
                   int obs_row,
                   int obs_col,
                   int orig_row,
                   int orig_col,
                   int orig_direction) {
  struct guard g;
  g.direction = orig_direction;
  g.position_r = orig_row;
  g.position_c = orig_col;

  set_in_array(*array, obs_row, obs_col, 'O');
  set_in_array(*array, g.position_r, g.position_c, '.');
  struct array2d visited_array = init_visited_array(array->numrows, array->numcols);
  int max_iter = 100000;
  int count = 0;
  while (g.position_r > 0 &&
         g.position_r < array->numrows &&
         g.position_c > 0 &&
         g.position_c < array->numcols) {
    
    set_in_array(visited_array, g.position_r, g.position_c, 1);
    move_guard(&g, array);
    /*printf("move guard to %d, %d\n", g.position_r, g.position_c);*/
    if (count > max_iter) {
      printf( "max iter reached\n");
      return 1;
    }

    count ++;
  }
  return 0;
}
int solve_v2(struct array2d array) {

  // determine the path the guard took
  struct guard g = init_guard(&array);
  struct array2d visited_array = init_visited_array(array.numrows, array.numcols);
  int r_orig = g.position_r;
  int c_orig = g.position_c;
  int d_orig = g.direction;

  set_in_array(visited_array, g.position_r, g.position_c, 1);
  set_in_array(array, g.position_r, g.position_c, '.');
  int max_iter = 100000;
  int count = 0;
  while (g.position_r > 0 &&
         g.position_r < array.numrows &&
         g.position_c > 0 &&
         g.position_c < array.numcols) {
    
    set_in_array(visited_array, g.position_r, g.position_c, 1);
    move_guard(&g, &array);
    /*printf("move guard to %d, %d\n", g.position_r, g.position_c);*/
    if (count > max_iter) {
      fprintf(stderr, "max iter reached\n");
      return 0;
    }

    count ++;
  }
  
  int num_options = 0;
  for (int r = 0; r < visited_array.numrows; r++) {
    for (int c = 0; c < visited_array.numcols; c++) {
      struct array2d new_arr = make_array(array.numrows, array.numcols);
      new_arr.arr = memcpy(new_arr.arr, array.arr, array.numrows * array.numcols * sizeof(char));
      if (get_value(visited_array, r, c, 0) > 0) {
        printf("checking %d, %d\n", r, c);
        num_options += check_for_loop(&new_arr,
                                      r,
                                      c,
                                      r_orig,
                                      c_orig,
                                      d_orig);
      }
    }
  }
  printf("num options = %d\n", num_options); 
  return num_options;
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
    fprintf(stderr, "Error opening file\n");
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
  struct line_info linfo = count_lines(fptr);

  // rewind the file back to the beginning
  rewind(fptr);

  // make a 2d array of all the characters
  struct array2d array = file_to_array(fptr, linfo);
  if (mode == 1) {
    total = solve_v1(array);
  }

  if (mode == 2) {
    total = solve_v2(array);
  }
  /*printf("total is %d\n", total);*/
}
