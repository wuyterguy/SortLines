#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <assert.h>

#define TEXT_FILE_NAME "text.txt"
#define MAX_LINES_COUNT 1000
#define MAX_LINE_SIZE 200

#include "qsort.c"

int GetNLines(char* lines[], int max_lines_count, FILE* file);


int main() {
    FILE* text_file = fopen(TEXT_FILE_NAME, "r");
    if (text_file == NULL) {
        printf("File \"" TEXT_FILE_NAME "\" doesn't exist or you don't have \"r\" permission\n");
        return 1;
    }

    char* lines[MAX_LINES_COUNT] = {};

    int lines_count = GetNLines(lines, MAX_LINES_COUNT, text_file);
}


int GetNLines(char* lines[], int max_lines_count, FILE* file) {
    assert(lines != NULL);
    assert(file != NULL);

    int lines_count = 0;

    char* buffer = NULL;
    size_t line_size = 0;
    for (char** i_ptr = lines;
         (i_ptr < lines + max_lines_count) && (getline(&buffer, &line_size, file) != EOF);
         ++i_ptr) {
        if (buffer == NULL) {
            printf("Memory error\n");
            exit(2);
        }

        *i_ptr = (char*)calloc(line_size, sizeof(char));
        strncpy(*i_ptr, buffer, line_size);
        lines_count++;
    }

    free(buffer);
}
