#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define BUFFER_SZ 50

//prototypes
void usage(char *);
void print_buff(char *, int);
int setup_buff(char *, char *, int);

int count_words(char *, int str_len);
void reverse_string(char *, int);
void print_words(char *, int);
int replace_string(char *, int, const char *, const char *);

int setup_buff(char *buff, char *user_str, int len) {
    char *dest = buff;
    char *src = user_str;
    int count = 0;
    int space_flag = 0;

    while (*src) {
        if (count >= len) return -1;

        if (isspace(*src)) {
            if (!space_flag) {
                *dest++ = ' ';
                count++;
                space_flag = 1;
            }
        } else {
        	// add non-whitespace character
            *dest++ = *src;
            count++;
            space_flag = 0;
        }
        src++;
    }

    // fill up remaining buffer with '.'
    memset(dest, '.', len - count);
    return count;
}

void print_buff(char *buff, int len){
    printf("Buffer:  ");
    for (int i = 0; i < len; i++){
        putchar(*(buff + i));
    }
    putchar('\n');
}

void usage(char *exename){
    printf("usage: %s [-h|c|r|w|x] \"string\" [other args]\n", exename);
}

int count_words(char *buff, int str_len) {
    int word_count = 0;
    int in_word = 0;

    for (char *ptr = buff; ptr < buff + str_len; ptr++) {
        if (!isspace(*ptr) && *ptr != '.') {
            // Start of a new word
            if (!in_word) {
                word_count++;
                in_word = 1;
            }
        } else {
        	// End of the current word
            in_word = 0;
        }
    }

    return word_count;
}

void reverse_string(char *buff, int str_len) {
    char *start = buff;
    char *end = buff + str_len - 1;
    char temp;

    while (start < end) {
        temp = *start;
        *start = *end;
        *end = temp;
        start++;
        end--;
    }

    printf("Reversed String: %.*s\n", str_len, buff);
}


void print_words(char *buff, int str_len) {
    int word_count = 0;
    int char_count = 0;
    int in_word = 0;

    printf("Word Print\n");
    printf("----------\n");

    for (char *ptr = buff; ptr < buff + str_len; ptr++) {
        char c = *ptr;

        if (!isspace(c) && c != '.') {
            if (!in_word) {
                // start of a new word
                word_count++;
                printf("%d. ", word_count);
                in_word = 1;
            }
            putchar(c);
            char_count++;
        } else {
            // end of a word
            if (in_word) {
                printf(" (%d)\n", char_count);
                char_count = 0;
                in_word = 0;
            }
        }
    }

    if (char_count > 0) {
        printf(" (%d)\n", char_count);
    }
}

int replace_string(char *buff, int len, const char *old, const char *new_str) {
    char *found = strstr(buff, old);
    if (!found) {
        printf("substring not found\n");
        return -1;
    }

    int old_len = strlen(old);
    int new_len = strlen(new_str);
    int buff_len = strlen(buff);

    if (buff_len - old_len + new_len > len) {
        printf("replacement causes buffer overflow\n");
        return -2; 
    }

    // create temporary buffer for result
    char temp[BUFFER_SZ];
    int prefix_len = found - buff;

    // copy the content before found substring
    strncpy(temp, buff, prefix_len);
    temp[prefix_len] = '\0';

    // append the replacement string
    strncat(temp, new_str, BUFFER_SZ - strlen(temp) - 1);

    // append content after found substring
    strncat(temp, found + old_len, BUFFER_SZ - strlen(temp) - 1);

    // copy result back into the original buffer
    strncpy(buff, temp, len);

    printf("Modified String: %.*s\n", len, buff);
    return 0; 
}

int main(int argc, char *argv[]) {

    char *buff;             //placeholder for the internal buffer
    char *input_string;     //holds the string provided by the user on cmd line
    char opt;               //used to capture user option from cmd line
    int rc;                 //used for return codes
    int user_str_len;       //length of user supplied string

    //TODO:  #1. WHY IS THIS SAFE, aka what if argv[1] does not exist?
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // This is safe because we check `argc < 2` to ensure there is at least one argument
    // beyond the program name before accessing `argv[1]`. If the argument doesn't exist,
    // we print usage and exit early to avoid undefined behavior.

    if ((argc < 2) || (*argv[1] != '-')) {
        usage(argv[0]);
        exit(1);
    }

    opt = (char)*(argv[1] + 1);   //get the option flag

    // Handle the help flag and exit normally
    if (opt == 'h') {
        usage(argv[0]);
        exit(0);
    }

    //TODO:  #2 Document the purpose of the if statement below
    //      PLACE A COMMENT BLOCK HERE EXPLAINING
    // This checks if the correct number of arguments are provided based on the flag.
    // For the '-x' flag, at least 5 arguments are required.
    // For other flags ('-c', '-r', '-w'), only 3 arguments are needed.
    if ((opt == 'x' && argc < 5) || (opt != 'x' && argc < 3)) {
        usage(argv[0]);
        exit(1);
    }

    input_string = argv[2]; //capture the user input string

    //TODO:  #3 Allocate space for the buffer using malloc and
    //          handle error if malloc fails by exiting with a 
    //          return code of 99
    buff = (char *)malloc(BUFFER_SZ);

    // Setup the buffer
    user_str_len = setup_buff(buff, input_string, BUFFER_SZ);
    if (user_str_len < 0) {
        // If the buffer setup fails, print an error message and exit
        printf("Error setting up buffer, error = %d\n", user_str_len);
        free(buff);
        exit(2);
    }

    // Handle the flag operations using a switch statement
    switch (opt) {
        case 'c': // Count words
            rc = count_words(buff, user_str_len); // Updated to match function signature
            if (rc < 0){
                printf("Error counting words, rc = %d", rc);
                exit(2);
            }
            printf("Word Count: %d\n", rc);
            break;

        case 'r': // Reverse string
            reverse_string(buff, user_str_len);
            break;

        case 'w': // Print words and their lengths
            print_words(buff, user_str_len);
            break;

        case 'x': // Replace substring
            if (argc < 5) {
                printf("-x requires three arguments\n");
                free(buff);
                exit(1);
            }
            rc = replace_string(buff, BUFFER_SZ, argv[3], argv[4]);
            if (rc < 0) {
                free(buff);
                exit(2);
            }
            break;

        default: // Invalid flag
            usage(argv[0]);
            free(buff);
            exit(1);
    }

    //TODO:  #6 Don't forget to free your buffer before exiting
    print_buff(buff, BUFFER_SZ);
    free(buff);
    exit(0);
}


//TODO:  #7  Notice all of the helper functions provided in the 
//          starter take both the buffer as well as the length.  Why
//          do you think providing both the pointer and the length
//          is a good practice, after all we know from main() that 
//          the buff variable will have exactly 50 bytes?
//  
//          PLACE YOUR ANSWER HERE

//          Passing both pointer (buff) and len ensures safety, flexibility, and clarity:
//             - Safety: Prevents buffer overflows through ensuring functions operate within the buffer's bounds.
//             - Flexibility: Makes helper functions reusable with buffers of different sizes.
//             - Clarity: Clearly documents the expected buffer size, aiding debugging and maintenance.
//          Even though BUFFER_SZ is fixed, this way future-proofs the code and prevents assumptions concerning the buffer size.

