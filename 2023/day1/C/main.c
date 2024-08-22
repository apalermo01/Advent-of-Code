#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <ctype.h>

char *reverse_string(const char *str);
struct word_pair{
    int num;
    char *name;
};

// function to concatenate integers
// https://stackoverflow.com/a/12700533
unsigned concatenate(unsigned x, unsigned y) {
    unsigned pow = 10;
    while (y >= pow) {
        pow *= 10;
    }
    return x * pow + y;
}

// define a function that processes a line
// input is a pointer to a character -> character array?
int process_line(char *line) {
    int first_num = 0;
    int second_num = 0;
    int found_first_int = 0;
    
    // handle a case where the line may not be null terminated.
    // unlikely here, but good to have just in case.
    size_t size = strnlen(line, 2048);
  
    for (int i = 0; i < size; i++) {

        if (isdigit(line[i]) && !found_first_int) {
            found_first_int = 1;
            // line[i] is a character, represented by an integer.
            // '0' is 48, '1' is 49, etc, so their difference is the actual number
            first_num = line[i] - '0';
            second_num = line[i] - '0';

        } else if (isdigit(line[i])) {
            second_num = line[i] - '0';
        }
    }

    unsigned result = concatenate(first_num, second_num);
    return result;
}

int check_num_in_substring(char *line, char *str_to_check, int num) {

    int num_chr_in_line = strlen(line);
    int num_chr_in_substring = strlen(str_to_check);
    int j = 0;

    // input validation
    if (num < 0 || num > 9) {
        fprintf(stderr, "expected integer representation of the number to check to be between 0 and 9 inclusive\n");
        return -2;
    }

    if (num_chr_in_line < num_chr_in_substring) {
        fprintf(stderr, "substring cannot be larger than the string we're searching in.\n");
        return -2;
    }

    // found the start of a match
    if (line[0] == str_to_check[0]) {

        // loop through indices in the substring
        for (j = 0; j < num_chr_in_substring; j++) {

            // check if there is not a match in substring
            if (line[j] != str_to_check[j]) {
                break;
            }
        }

        // If j is the same length as the number of characters we had to check,
        // then that means all characters match and we've matched the string
        if (j == num_chr_in_substring) {
            return num;
        }
    }
    return -1;
}

struct word_pair check_for_str_nums(char *line, int use_reversed) {

    if ((use_reversed != 0) && (use_reversed != 1)) {
        fprintf(stderr, "expected 0 or 1 for use_reversed, got %d\n", use_reversed);
        struct word_pair ret = {-1, NULL};
        return ret;
    }

    int nums[] = {0, 1, 2, 3, 4, 5, 6, 7, 8, 9};

    // strs is an array of pointers to a char (beginning of the string)
    char *strs[] = {"zero", "one", "two", "three", "four", "five", "six", "seven", "eight", "nine"};

    
    if (use_reversed == 1) {
        for (int i = 0; i < 10; i++) {
            char *reversed = reverse_string(strs[i]);
            if (reversed != NULL) {
                strs[i] = reversed;
            } else {
                fprintf(stderr, "Memory allocation failed for reversing strings");
                struct word_pair ret = {-1, "zero"};
                return ret;
            }
        }
    }

    int res;

    for (int i = 0; i < 10; i++) {
        if (strlen(line) >= strlen(strs[i])) {
            res = check_num_in_substring(line, strs[i], nums[i]);
            if (res >= 0) {
                struct word_pair ret = {res, strs[i]};
                return ret;
            }
        }
    }

    struct word_pair ret = {-1, NULL};
    return ret;
}


/* Get a slice of a string
 * copies the contents of src to dest starting from index start up to, and including, end
 */
char * get_slice(char *src, const size_t start, const size_t end) {
    
    // input validation
    if (end < strlen(src)) {
        fprintf(stderr, "ERROR: attempting to slice past the last character in the string\n"); 
        fprintf(stderr, "Substring: %s, start: %d, end: %d, strlen = %d\n", src, start, end, strlen(src));
        return NULL;
    }
    
    char *dest[end-start];
    for (int i = start; i < end; i++) {
        dest[i - start] = &src[i];

        // shouldn't happen because of the input validation above,
        // but just in case
        if (src[i] == '\0') {
            break;
        }
    }

    return *dest;
    
}

char * reverse_string(const char *str) {
    size_t len = strlen(str);

    // allocating a character array for the destination string
    char *dest = (char *)malloc(len + 1);

    if (dest == NULL) {
        fprintf(stderr, "memory allocation failed");
        return NULL;
    }

    for (size_t i = 0; i < len; i++) {
        dest[i] = str[len - 1 - i];
    }

    dest[len] = '\0';
    
    return dest;
}

int find_next_number(char *line, int use_reversed) {

    struct word_pair wp;
    
    int num = -1;
    size_t size = strlen(line);

    for (int i = 0; i < size; i++) {
        // check for integer
        if (isdigit(line[i])) {
            num = line[i] - '0';
            return num;
        }

        // check for spelled out number
        char *slice = get_slice(line, i, strlen(line));
        wp = check_for_str_nums(slice, use_reversed);

        if (wp.num > -1) {
            return wp.num;
        }
    }

    return -1;
}

int process_line_version_2(char *line) {

    int first_num = -1;
    int second_num = -1;

    // loop forward and find the first number
    first_num = find_next_number(line, 0);
    if (first_num == -1) first_num = 0;

    // reverse the string
    char *line_reversed = reverse_string(line);
    if (line_reversed != NULL) {

        // loop backward and find the last number
        second_num = find_next_number(line_reversed, 1);
        if (second_num == -1) second_num = first_num;
        free(line_reversed);
    } else {
        fprintf(stderr, "Error allocating memory");
    }

    unsigned result = concatenate(first_num, second_num);
    return result;

}

int main() {
    
    char text[255];
    int total = 0;
    FILE *fptr;

    if ((fptr = fopen("../input_1.txt", "r")) == NULL) {
        printf("Error opening file");

        free(fptr);
        exit(1);
    }

    printf("file opening successful\n");

    while (fgets(text, sizeof(text), fptr)) {
        int num_this_line = process_line(text);
        total += num_this_line;
    }
    fclose(fptr);
    
    printf("total version 1: %d\n", total);
    
    /* Second part */
    total = 0;
    if ((fptr = fopen("../input_1.txt", "r")) == NULL) {
        printf("Error opening file");

        free(fptr);
        exit(1);
    }

    printf("file opening successful\n");

    while (fgets(text, sizeof(text), fptr)) {
        int num_this_line = process_line_version_2(text);
        total += num_this_line;
    }
    fclose(fptr);
    
    printf("total version 2: %d\n", total);
    return 0;
}
