#include "./utils.c"
#include <ctype.h>
#include <string.h>

void print_int_buff(const int* buff, const int len) {
  for(int i = 0; i < len; i++) {
    printf("%d, ", buff[i]);
  }
  printf("\n");
}

long solve_v1(int* buff, int len) {
  long checksum = 0;
  int dest_ptr = 1; // pointer to a free space to write memory to
  int read_ptr = 0; // pointer to the chunk we're reading from
  int mem_addr = buff[0]; // move the memory tracker straight to the first block 
  // of free space since the position id is 0
  
  // check if ending block is just free space
  if (len % 2 == 0) {
    len --;
  }

  int max_iter = 100000;
  int iter = 0;
  for (read_ptr = len-1; read_ptr >= 0; read_ptr -=2) {
    // move on to the next item when there's no more memory in this chunk
    while(buff[read_ptr] > 0) {

      // move destination forward when there's no more free space
      if (buff[dest_ptr] == 0) {
        // also increment the memory pointer over existing memory 
        for (int y = 0; y < buff[dest_ptr+1]; y++) {

          checksum += (mem_addr * ((dest_ptr+1)/2));
          mem_addr ++;
        }
        printf("\n");
        // potentially increment multiple times if there is 0 free space
        while(buff[dest_ptr] == 0 && dest_ptr < len) {
          dest_ptr +=2;
          printf("dest ptr = %d\n", dest_ptr);
        }
      }
      
      // if dest meets read, then we're done moving chunks of memory
      if (dest_ptr >= read_ptr) {
        break;
      }
      buff[dest_ptr] --;
      buff[read_ptr] --;

      if (buff[dest_ptr] < 0 || buff[read_ptr] < 0 ) {
        printf("ERROR; buff[%d] = %d, buff[%d] = %d\n", dest_ptr, buff[dest_ptr], read_ptr, buff[read_ptr]);
        exit(1);
        break;
      }
      checksum += (mem_addr * (read_ptr/2));
      mem_addr ++;
      if (iter > max_iter) {
        printf("max iter reached\n");
        break;
      }

      iter ++;
    }
    if (dest_ptr >= read_ptr) {
      break;
    }
  }
  
  // right now, we've computed the checksum for the built up continguous block 
  // of memory, however ther's still the memory that we weren't able to move
  // since none of this memory has been moved, the id should still correspond to 
  // dest_ptr
  
  // dest ptr is most likely pointing at an empty chunk in the filesystem. Increment 
  // it to be at the next bit of memory 
  if (dest_ptr % 2 == 1) {
    dest_ptr ++;
  }
  while (dest_ptr <= len) {
    for (int i = buff[dest_ptr]; i > 0; i--) {
      checksum += (mem_addr * dest_ptr);
      mem_addr ++;
    } 

    dest_ptr += 2;
  }

  printf("dest ptr = %d\n", dest_ptr);
  printf("buff =");
  for (int a = 0; a < len; a++) {
    printf("%d, ", buff[a]);
  }
  printf("\n");
  printf("checksum is %ld\n", checksum);
  return checksum;
}

int calc_memory_repr_size(const int* buff, int len) {
  int size = 0;

  for (int i = 0; i < len; i++) {
    size += buff[i];
  }

  return size;
}

struct memchunk {
  int id;
};

void print_mem_repr(const struct memchunk* m, int size) {
  for (int i = 0; i < size; i++) {
    if (m[i].id == -1) {
      printf("%c", '.');
    } else {
      printf("%d", m[i].id);
    }
  }
}

void init_memory_repr(const int* buff, int len, struct memchunk* dest) {
  int index = 0;
  for (int i = 0; i < len; i++) {
    // allocate memory 
    if (i % 2 == 0) {
      for (int j = 0; j < buff[i]; j++) {
        struct memchunk m;
        m.id = i / 2;
        dest[index] = m;
        index ++;
      } 
    }

    // allocate blank space 
    if (i % 2 == 1) {
      for (int j = 0; j < buff[i]; j++) {
        struct memchunk m;
        m.id = -1;
        dest[index] = m;
        index ++;
      }
    }
  }
}

void trade_memchunks(struct memchunk* mem_repr, int write_ptr, int read_ptr) {
  struct memchunk temp = mem_repr[write_ptr];
  mem_repr[write_ptr] = mem_repr[read_ptr];
  mem_repr[read_ptr] = temp;
}

long solve_v2(int* buff, int len) {
  long checksum = 0;
  int size = calc_memory_repr_size(buff, len);
  struct memchunk* mem_repr = malloc(size * sizeof(struct memchunk));
  if (mem_repr == NULL) {
    fprintf(stderr, "failed to allocate memory\n");
    exit(1);
  }

  init_memory_repr(buff, len, mem_repr);

  int read_ptr = size - 1;
  
  for (int write_ptr = 0; write_ptr < size; write_ptr++) {
    if (mem_repr[write_ptr].id == -1) {
      trade_memchunks(mem_repr, write_ptr, read_ptr);
      do {
      read_ptr --;
      } while (mem_repr[read_ptr].id == -1);
    } else {
    }
    if (write_ptr >= read_ptr) {
      break;
    }
  }

  for (int i = 0; i < size; i++) {
    if (mem_repr[i].id != -1) {
      checksum += (i * mem_repr[i].id);
    }
  }

  printf("checksum = %ld\n", checksum);
  return checksum;
}

struct filechunk {
  int id;
  int size;
  int is_file; // 0 for empty space, 1 for file
};

void print_file_chunk(struct filechunk f) {
  if(f.id == -1) {
    printf(".");
  } else {
    for (int i = 0; i < f.size; i++) {
      printf("%d", f.id);
    }
  }

  printf(" ");
}

void init_file_repr(const int* buff, int len, struct filechunk* dest) {
  for (int i = 0; i < len; i++) {
    // allocate memory 
    if (i % 2 == 0) {
      struct filechunk f;
      f.id = i / 2;
      f.size = buff[i];
      f.is_file = 1;
      dest[i] = f;
    }

    // allocate blank space 
    if (i % 2 == 1) {
      struct filechunk f;
      f.id = -1;
      f.size = buff[i];
      f.is_file = 0;
      dest[i] = f;
    }
  }
}

void print_mem_repr_from_file(struct filechunk* file_repr, int len) {
  int size = 0;
  for (int i = 0; i < len; i++) {
    size += file_repr[i].size;
  }

  struct memchunk* mem_repr = malloc(size * sizeof(struct memchunk));
  if (mem_repr == NULL) {
    fprintf(stderr, "failed to allocate memory\n");
    exit(1);
  }
  
  int index = 0;
  for (int i = 0; i < len; i++) {
    for (int j = 0; j < file_repr[i].size; j++) {
      struct memchunk m;
      m.id = file_repr[i].id;
      mem_repr[index] = m;
      index ++;
    }
  }
  
  print_mem_repr(mem_repr, index);
  free(mem_repr);
}

struct filechunk* trade_filechunk(struct filechunk* file_repr,
                                  int write_ptr,
                                  int read_ptr,
                                  int* len) {
  int size_diff = file_repr[write_ptr].size - file_repr[read_ptr].size;
  struct filechunk temp = file_repr[write_ptr];
  file_repr[write_ptr] = file_repr[read_ptr];
  temp.size -= size_diff;
  file_repr[read_ptr] = temp;
  
  // Insert a new blank space based on the size difference
  struct filechunk* file_repr_new = malloc((*len + 1) * sizeof(struct filechunk));
  if (file_repr == NULL) {
    fprintf(stderr, "error reallocating memory \n");
    exit(1);
  }

  struct filechunk f;
  f.size = size_diff;
  f.id = -1;
  f.is_file = 0;
  
  for (int i = 0; i <= write_ptr; i++) {
    file_repr_new[i] = file_repr[i];
  }

  file_repr_new[write_ptr+1] = f;

  for (int i = write_ptr + 1; i < *len; i++) {
    if (i == read_ptr) {
    }
    file_repr_new[i+1] = file_repr[i];
  }

  free(file_repr);
  (*len) ++;
  return file_repr_new;
}


long solve_v3(int* buff, int len) {
  long checksum = 0;
  struct filechunk* file_repr = malloc(len * sizeof(struct filechunk));
  if (file_repr == NULL) {
    fprintf(stderr, "failed to allocate memory\n");
    exit(1);
  }

  init_file_repr(buff, len, file_repr);
  
  int write_ptr = 0;
  for (int read_ptr = len-1; read_ptr >= 0; read_ptr --) {
    for (int write_ptr = 0; write_ptr < read_ptr; write_ptr ++) {
      if ((file_repr[write_ptr].id == -1) &&
          (file_repr[read_ptr].id != -1) &&
          (file_repr[write_ptr].size >= file_repr[read_ptr].size)) {
          file_repr = trade_filechunk(file_repr, write_ptr, read_ptr, &len);
          break;
        }
      }
  }

  int index = 0;
  for (int i = 0; i < len; i++) {

    if (file_repr[i].id != -1) {
      for (int j = 0; j < file_repr[i].size; j ++) {
        checksum += (index * file_repr[i].id);
        index ++;
      }
    } else {
      index += file_repr[i].size;
    }
  }
  free(file_repr);
  printf("checksum = %ld\n", checksum);
  return checksum;
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
  
  char* str_buff;
  ssize_t read;
  size_t len = 0;

  read = getline(&str_buff, &len, fptr);
  if (read == -1) {
    fprintf(stderr, "error reading file\n");
  }
  
  int string_len = 0;
  for (int i = 0; i < len; i++) {
    if (str_buff[i] != '\0') {
      string_len ++;
    } else {
      break;
    }
  }

  string_len --;

  int* buff = malloc(string_len * sizeof(int));
  for (int i = 0; i < string_len; i++) {
    buff[i] = (int) (str_buff[i] - 48);
  }
  if (mode == 1) {
    solve_v2(buff, string_len);
  }

  if (mode == 2) {
    solve_v3(buff, string_len);
  }
}

