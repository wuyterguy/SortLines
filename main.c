#define _POSIX_C_SOURCE _GNU_SOURCE
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <unistd.h>
#include "qsort.c"

#define TEXT_FILE_NAME "onegin.txt"
#define OUTPUT_FILE_NAME "output.txt"
#define MAX_LINES_COUNT 1000
#define MAX_LINE_SIZE 200

enum {MAIN_NORMAL = 0, MAIN_MEMORY_ERROR, MAIN_OPEN_ERROR};
typedef enum {PRINT_NORMAL = 0, PRINT_OPEN_ERROR}     Print_err_t; // return of PrintResult()
typedef enum {CREATE_NORMAL = 0, CREATE_MEMORY_ERROR} Create_err_t; // return of CreateIndexes()

typedef char** Index_t;

struct Text_file_t {ssize_t buffer_size; char* buffer; size_t text_size;
                    Index_t index; int lines_count; size_t longest_line_size;};

Text_file_t GetText(const char* filename);

Index_t GetLines(char* buffer, Text_file_t* p_text_file);

Create_err_t CreateIndexes(Index_t* first_letters_index, Index_t* last_letters_index,
                           size_t lines_count);

int FirstLettersAlphabetCmp(const void* str_ptr1, const void* str_ptr2);

int LastLettersAlphabetCmp(const void* str_ptr1, const void* str_ptr2);

Print_err_t PrintResult(Text_file_t* p_text_file, Index_t first_letters_index, Index_t last_letters_index,
                 const char* output_file_name);


int main() {
    Text_file_t text_file = GetText(TEXT_FILE_NAME);

    Index_t first_letters_index = NULL;
    Index_t last_letters_index = NULL;
    if (CreateIndexes(&first_letters_index, &last_letters_index, text_file.lines_count) != CREATE_NORMAL) {
        free(text_file.buffer); free(text_file.index);
        return MAIN_MEMORY_ERROR;
    }
    WriteIn(first_letters_index, text_file.index, text_file.lines_count * sizeof(char*));
    WriteIn(last_letters_index, text_file.index, text_file.lines_count * sizeof(char*));

    MYqsort(first_letters_index, first_letters_index + text_file.lines_count - 1, sizeof(char*),
            UpgradedHoarePartition, FirstLettersAlphabetCmp);
    qsort(last_letters_index, text_file.lines_count, sizeof(char*), LastLettersAlphabetCmp);

    if (PrintResult(&text_file, first_letters_index, last_letters_index, OUTPUT_FILE_NAME) != PRINT_NORMAL) {
        free(text_file.buffer); free(text_file.index); free(first_letters_index); free(last_letters_index);
        return MAIN_OPEN_ERROR;
    }

    free(text_file.buffer); free(text_file.index); free(first_letters_index); free(last_letters_index);
    return MAIN_NORMAL;
}


Text_file_t GetText(const char* filename) {
    assert(filename != NULL);

    struct stat file_stat = {};
    stat(filename, &file_stat);

    Text_file_t text_file = {.buffer_size = file_stat.st_size, .buffer = NULL, .text_size = 0,
                                    .index = NULL, .lines_count = 0, .longest_line_size = 0};

    text_file.buffer = (char*)calloc(text_file.buffer_size, sizeof(char));
    if (text_file.buffer == NULL) {
        printf("GetText(): Failed to allocate memory for text_file.buffer, line %d\n", __LINE__);
        exit(MAIN_MEMORY_ERROR);
    }

    int file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor == -1) {
        printf("GetText(): Failed to open %s with mode O_RDONLY, line %d\n", filename, __LINE__);
        free(text_file.buffer);
        exit(MAIN_OPEN_ERROR);
    }

    text_file.text_size = read(file_descriptor, text_file.buffer, file_stat.st_size);
    text_file.buffer[text_file.text_size] = '\0';

    close(file_descriptor);

    text_file.index = GetLines(text_file.buffer, &text_file);

    return text_file;
}


Index_t GetLines(char* buffer, Text_file_t* p_text_file) {
    assert(buffer != NULL); assert(p_text_file != NULL);

    size_t line_size = 0;
    for (char* i_ptr = buffer; *i_ptr != '\0'; i_ptr++) {
        line_size++;
        if (*i_ptr == '\n' or *(i_ptr + 1) == '\0') {       // NOTE (*i_ptr == '\0') is there for case when last line don't has \n
            if (line_size > p_text_file->longest_line_size)
                p_text_file->longest_line_size = line_size;
            line_size = 0;

            (p_text_file->lines_count)++;
        }
    }

    Index_t index = (Index_t)calloc(p_text_file->lines_count, sizeof(char*));
    if (index == NULL) {
        printf("GetLines(): Failed to allocate memory for index, line %d\n", __LINE__);
        free(buffer);
        exit(MAIN_MEMORY_ERROR);
    }

    Index_t in_index_pos = index;
    char* last_line_start = buffer;
    for (char* i_ptr = buffer + 1; *i_ptr != '\0'; i_ptr++) {
        if ((*i_ptr == '\n') || (*(i_ptr + 1) == '\0')) {   // NOTE (*i_ptr == '\0') is there for case when last line don't has \n
            *in_index_pos++ = last_line_start;
            last_line_start = i_ptr + 1;
            *i_ptr = '\0';
        }
    }

    return index;
}


Create_err_t CreateIndexes(Index_t* p_first_letters_index, Index_t* p_last_letters_index,
                           size_t lines_count) {
    assert(p_first_letters_index != NULL); assert(p_last_letters_index != NULL);

    *p_first_letters_index = (Index_t)calloc(lines_count, sizeof(char*));
    if (p_first_letters_index == NULL) {
        printf("CreateIndexes(): Failed to allocate memory for first_letters_index, line %d\n", __LINE__);
        return CREATE_MEMORY_ERROR;
    }
    *p_last_letters_index = (Index_t)calloc(lines_count, sizeof(char*));
    if (p_last_letters_index == NULL) {
        printf("CreateIndexes(): Failed to allocate memory for last_letters_index, line %d\n", __LINE__);
        free(*p_first_letters_index);
        return CREATE_MEMORY_ERROR;
    }

    return CREATE_NORMAL;
}


int FirstLettersAlphabetCmp(const void* str1_ptr, const void* str2_ptr) {
    assert(str1_ptr != NULL); assert(str2_ptr != NULL);

    const char* in_str1_pos = *(const char**)str1_ptr;
    const char* in_str2_pos = *(const char**)str2_ptr;

    while ((*in_str1_pos != '\0') || (*in_str2_pos != '\0')) {
        for (; !isalpha(*in_str1_pos) && (*in_str1_pos != '\0'); in_str1_pos++);
        for (; !isalpha(*in_str2_pos) && (*in_str2_pos != '\0'); in_str2_pos++);

        if (tolower(*in_str1_pos) > tolower(*in_str2_pos))
            return 1;
        if (tolower(*in_str1_pos) < tolower(*in_str2_pos))
            return -1;

        in_str1_pos++;
        in_str2_pos++;
    }

    return 0;
}


int LastLettersAlphabetCmp(const void* str1_ptr, const void* str2_ptr) {
    assert(str1_ptr != NULL); assert(str2_ptr != NULL);

    const char* in_str1_pos = *(const char**)str1_ptr;
    const char* in_str2_pos = *(const char**)str2_ptr;

    size_t str1_size = 1;   //NOTE there is 1 because just '\0' is line with size 1
    size_t str2_size = 1;

    for (; *in_str1_pos != '\0'; str1_size++, in_str1_pos++);
    for (; *in_str2_pos != '\0'; str2_size++, in_str2_pos++);

    while ((str1_size > 0) && (str2_size > 0)) {
        for (; !isalpha(*in_str1_pos) && (str1_size > 0); str1_size--, in_str1_pos--);
        for (; !isalpha(*in_str2_pos) && (str2_size > 0); str2_size-- , in_str2_pos--);

        if (tolower(*in_str1_pos) > tolower(*in_str2_pos))
            return 1;
        if (tolower(*in_str1_pos) < tolower(*in_str2_pos))
            return -1;

        in_str1_pos--;
        in_str2_pos--;
    }

    if (str1_size > 0)
        return 1;
    else if (str2_size > 0)
        return -1;
    else
        return 0;
}


Print_err_t PrintResult(Text_file_t* p_text_file, Index_t first_letters_index, Index_t last_letters_index,
                 const char* output_file_name) {
    assert(p_text_file != NULL); assert(first_letters_index != NULL); assert(last_letters_index != NULL); assert(output_file_name != NULL);

    int output_file_descriptor = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_file_descriptor == -1) {
        printf("PrintResult(): Failed to open %s with mode O_WRONLY | O_CREAT | O_TRUNC, line %d\n",
               output_file_name, __LINE__);\
        return PRINT_OPEN_ERROR;
    }

    if (dup2(output_file_descriptor, STDOUT_FILENO)) {
        printf("PrintResult(): dup2 error, line %d\n", __LINE__);
        close(output_file_descriptor);
        return PRINT_OPEN_ERROR;
    }

    for(int i = 0; i < p_text_file->lines_count; i++)
        printf("%*s\t%*s\t%*s\n", -p_text_file->longest_line_size, first_letters_index[i],
                                  -p_text_file->longest_line_size, last_letters_index[i],
                                  -p_text_file->longest_line_size, p_text_file->index[i]);

    close(output_file_descriptor);
    return PRINT_NORMAL;
}
