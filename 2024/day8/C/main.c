#include "./utils.c"
#include <ctype.h>
#include <string.h>

int count_nodes(struct array2d array,
                int r_second,
                int c_second,
                int r_first,
                int c_first,
                struct array2d* visited_array) {
  int count = 0;
  int node1_r = (r_first * 2) - r_second;
  int node1_c = (c_first * 2) - c_second;

  int node2_r = (r_second * 2) - r_first;
  int node2_c = (c_second * 2) - c_first;
  /*printf("second: (%d, %d), first: (%d, %d)\n", r_second, c_second, r_first, c_first);*/
  /*printf("first node = (%d, %d)\n",*/
  /*        (r_first * 2) - r_second,*/
  /*        (c_first * 2) - c_second*/
  /*       );*/
  /*printf("second node = (%d, %d)\n",*/
  /*        (r_second * 2) - r_first,*/
  /*        (c_second * 2) - c_first*/
  /*       );*/
  /*if(((r_first * 2) - r_second > 0) && ((c_first * 2) - c_second > 0)) {*/
  if ((node1_r >= 0) &&
      (node1_r < array.numrows) &&
      (node1_c >= 0) &&
      (node1_c < array.numcols) &&
      (get_value(*visited_array, node1_r, node1_c, 0) == 0)) {
    /*printf("antinode (1) at r=%d, c=%d\n", node1_r, node1_c);*/
    set_in_array(*visited_array, node1_r, node1_c, 1);
    count ++;
  }
  if ((node2_r >= 0) &&
      (node2_r < array.numrows) &&
      (node2_c >= 0) &&
      (node2_c < array.numcols) &&
      (get_value(*visited_array, node2_r, node2_c, 0) == 0)) {
    /*printf("antinode (2) at r=%d, c=%d\n", node2_r, node2_c);*/
    set_in_array(*visited_array, node2_r, node2_c, 1);
    count ++;
  }

  return count;
}

int find_antinodes(struct array2d array,
                   int r,
                   int c,
                   char ch,
                   struct array2d visited_array) {

  /*printf("finding antinodes at r=%d, c=%d\n", r, c);*/
  int num_nodes = 0;
  for (int r_loop = 0; r_loop < array.numrows; r_loop++) {
    for (int c_loop = 0; c_loop < array.numcols; c_loop++) {

      // get the tracker up to where we stopped at ch
      if ((r_loop < r) || (((r_loop == r) && (c_loop <= c)))) {
        continue;
      }

      // stop when we hit another antenna of the same frequency
      if(get_value(array, r_loop, c_loop, 0) == ch){
        /*printf("counting nodes at %d, %d\n", r_loop, c_loop);*/
        num_nodes += count_nodes(array, r_loop, c_loop, r, c, &visited_array);

      }
    }
  }

  return num_nodes;
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

int solve_v1(struct array2d array) {
  char ch;
  int total = 0;
  struct array2d visited_array = init_visited_array(array.numrows, array.numcols);
  for (int r = 0; r < array.numrows; r++){
    for (int c = 0; c < array.numcols; c++){
      ch = get_value(array, r, c, 0);
      if (ch != '.' && ch != '#') {
        total += find_antinodes(array, r, c, ch, visited_array);
      }
    }
  } 

  printf("num nodes = %d\n", total);
  return 0;
}

int count_all_nodes(struct array2d array,
                int r_second,
                int c_second,
                int r_first,
                int c_first,
                struct array2d* visited_array) {
  int count = 0;
  int node_r = r_first;
  int node_c = c_first;
  int mult = 0;

  while (node_r >= 0 &&
         node_r < array.numrows &&
         node_c >= 0 &&
         node_c < array.numcols) {
    
    if (get_value(*visited_array, node_r, node_c, 0) == 0) {
      set_in_array(*visited_array, node_r, node_c, 1);
      /*printf("found antinode at r=%d, c=%d\n", node_r, node_c);*/
      count ++;
    }
    node_r = r_first + ((r_second - r_first) * mult);
    node_c = c_first + ((c_second - c_first) * mult);
    mult ++;
  }

  mult = 0;
  node_r = r_first;
  node_c = c_first;
  while (node_r >= 0 &&
         node_r < array.numrows &&
         node_c >= 0 &&
         node_c < array.numcols) {
    
    if (get_value(*visited_array, node_r, node_c, 0) == 0) {
      set_in_array(*visited_array, node_r, node_c, 1);
      /*printf("found antinode at r=%d, c=%d\n", node_r, node_c);*/
      count ++;
    }
    node_r = r_first + ((r_second - r_first) * mult);
    node_c = c_first + ((c_second - c_first) * mult);
    mult --;
  }
  
  return count;
  return count;
}
int find_all_antinodes(struct array2d array,
                   int r,
                   int c,
                   char ch,
                   struct array2d visited_array) {

  /*printf("finding antinodes at r=%d, c=%d\n", r, c);*/
  int num_nodes = 0;
  for (int r_loop = 0; r_loop < array.numrows; r_loop++) {
    for (int c_loop = 0; c_loop < array.numcols; c_loop++) {

      // get the tracker up to where we stopped at ch
      if ((r_loop < r) || (((r_loop == r) && (c_loop <= c)))) {
        continue;
      }

      // stop when we hit another antenna of the same frequency
      if(get_value(array, r_loop, c_loop, 0) == ch){
        /*printf("counting nodes at %d, %d\n", r_loop, c_loop);*/
        num_nodes += count_all_nodes(array, r_loop, c_loop, r, c, &visited_array);

      }
    }
  }

  return num_nodes;
}
int solve_v2(struct array2d array) {
  char ch;
  int total = 0;
  struct array2d visited_array = init_visited_array(array.numrows, array.numcols);
  for (int r = 0; r < array.numrows; r++){
    for (int c = 0; c < array.numcols; c++){
      ch = get_value(array, r, c, 0);
      if (ch != '.' && ch != '#') {
        total += find_all_antinodes(array, r, c, ch, visited_array);
      }
    }
  } 

  printf("num nodes = %d\n", total);
  return 0;
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

  /*if (mode == 2) {*/
  /*  total = solve_v2(array);*/
  /*}*/
  /*printf("total is %d\n", total);*/
}

// 509 -> too high
// 338 -> too high
