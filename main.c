#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h> // TODO add to readme
#include "qsort.c"

#define TEXT_FILE_NAME "onegin.txt"
#define OUTPUT_FILE_NAME "output.txt"

enum {MAIN_NORMAL = 0, MAIN_TEXT_ERROR = 1, MAIN_CREATE_ERROR = 2, MAIN_PRINT_ERROR = 3}; // return of main()
typedef enum {TEXT_NORMAL = 0, TEXT_MEMORY_ERROR = 1,
              TEXT_OPEN_ERROR = 2, TEXT_READ_ERROR = 3}   Text_err_t;   // field of structure returned by GetIndex()
typedef enum {INDEX_NORMAL = 0, INDEX_MEMORY_ERROR = 1}   Index_err_t;  // return of GetIndex()
typedef enum {CREATE_NORMAL = 0, CREATE_MEMORY_ERROR = 1} Create_err_t; // return of CreateIndexes()
typedef enum {PRINT_NORMAL = 0, PRINT_OPEN_ERROR = 2}     Print_err_t;  // return of PrintResult()

typedef enum {PAIRWISE, CROSS, NESTED} Rhyme_t;

struct Line_pos {char* line_start; char* line_end;};

typedef Line_pos* Index_t;

struct Text_file_t {ssize_t buffer_size; char* buffer; size_t text_size;
                    int lines_count; size_t longest_line_size; Index_t index;
                    Text_err_t error_code;};

/**
 * @brief free some fields of Text_file_t structure
 *
 * @param[in] p_text_file pointer to Text_file_t structure
 */
void DestructTextFile (Text_file_t* p_text_file);

/**
 * @brief get Text_file_t structure for file with text
 *
 * @param[in] filename name of the file
 *
 * @return Text_file_t structure
 */
Text_file_t GetText(const char* filename);


/**
 * @brief calculate and fill Text_file_t structure with .longest_line_size, .lines_count
 *        and modify buffer with '\0'
 *
 * @param[in, out] p_text_file pointer to Text_file_t structure
 */
void GetLines(Text_file_t* p_text_file);

/**
 * @brief fill Text_file_t structure with .index
 *
 * @param[in, out] p_text_file pointer to Text_file_t structure
 */
Index_err_t GetIndex(Text_file_t* p_text_file);

/**
 * @brief print roots depending on the value of n_roots
 *
 * @param[out] p_first_letters_index pointer to variable storing first letters index array start
 * @param[out] p_last_letters_index  pointer to variable storing last letters index array start
 * @param[in]  p_text_file           pointer to Text_file_t structure
 *
 * @return CREATE_NORMAL or error number
 */
Create_err_t CreateIndexes(Index_t* p_first_letters_index, Index_t* p_last_letters_index,
                           Text_file_t* p_text_file);

/**
 * @brief alphabetically comparing from the first letter
 *
 * @param[in] str1 first being compared line
 * @param[in] str2 first being compared line
 *
 * @return 1 if str1 > str2, -1 if str1 < str2, 0 if str1 = str2
 */
int FirstLettersAlphabetCmp(const void* str1, const void* str2);

/**
 * @brief alphabetically comparing from the last letter
 *
 * @param[in] str1 first being compared line
 * @param[in] str2 first being compared line
 *
 * @return 1 if str1 > str2, -1 if str1 < str2, 0 if str1 = str2
 */
int LastLettersAlphabetCmp(const void* str1, const void* str2);

/**
 * @brief print result of sorting in three columns
 *
 * @param[in] p_text_file         pointer to Text_file_t structure
 * @param[in] first_letters_index first letters index array
 * @param[in] last_letters_index  last letters index array
 * @param[in] output_file_name    name of file that it print in
 *
 * @return PRINT_NORMAL or error number
 */
Print_err_t PrintResult(Text_file_t* p_text_file, Index_t first_letters_index, Index_t last_letters_index,
                 const char* output_file_name);

/**
 * @brief print error message
 *
 * @param[in] message error message
 * @param[in] object  object of error
 */
void Print_error_message(const char* message, const char* object);

/*Index_t NonsenseGenerator (Index_t rhymed_index, int rhymed lines count,
                           Rhyme_t rhyme_type, int nonsense_quatrain_count);*/


int main() {
    Text_file_t text_file = GetText(TEXT_FILE_NAME);
    if (text_file.error_code != TEXT_NORMAL)
        return MAIN_TEXT_ERROR;

    Index_t first_letters_index = NULL;
    Index_t last_letters_index = NULL;
    if (CreateIndexes(&first_letters_index, &last_letters_index, &text_file) != CREATE_NORMAL) {
        DestructTextFile(&text_file);
        return MAIN_CREATE_ERROR;
    }

    MYqsort(first_letters_index, first_letters_index + text_file.lines_count - 1, sizeof(Line_pos),
            UpgradedHoarePartition, FirstLettersAlphabetCmp);
    qsort(last_letters_index, text_file.lines_count, sizeof(Line_pos), LastLettersAlphabetCmp);

    if (PrintResult(&text_file, first_letters_index, last_letters_index, OUTPUT_FILE_NAME) != PRINT_NORMAL) {
        DestructTextFile(&text_file); free(first_letters_index); free(last_letters_index);
        return MAIN_PRINT_ERROR;
    }

    DestructTextFile(&text_file); free(first_letters_index); free(last_letters_index);
    return MAIN_NORMAL;
}


Text_file_t GetText(const char* filename) {
    assert(filename != NULL);

    struct stat file_stat = {};
    stat(filename, &file_stat);

    Text_file_t text_file = {.buffer_size = file_stat.st_size, .buffer = NULL, .text_size = 0,
                             .lines_count = 0, .longest_line_size = 0, .index = NULL,
                             .error_code = TEXT_NORMAL};

    text_file.buffer = (char*)calloc(text_file.buffer_size, sizeof(char));
    if (text_file.buffer == NULL) {
        Print_error_message("Failed to allocate memory", "text_file.buffer");
        text_file.error_code = TEXT_MEMORY_ERROR;
        return text_file;
    }

    int file_descriptor = open(filename, O_RDONLY);
    if (file_descriptor < 0) {
        Print_error_message("Failed to open with mode O_RDONLY", filename);
        free(text_file.buffer);
        text_file.error_code = TEXT_OPEN_ERROR;
        return text_file;
    }

    text_file.text_size = read(file_descriptor, text_file.buffer, file_stat.st_size);
    if (text_file.text_size < 0) {
        Print_error_message("reading file error", filename);
        free(text_file.buffer);
        close(file_descriptor);
        text_file.error_code = TEXT_READ_ERROR;
        return text_file;
    }
    text_file.buffer[text_file.text_size] = '\0';

    close(file_descriptor);

    GetLines(&text_file); // NOTE fill .longest_line_size, .lines_count; modify buffer with '\0'
    if (GetIndex(&text_file) != INDEX_NORMAL) { // NOTE fill .index
        Print_error_message("reading file error", filename);
        free(text_file.buffer);
        text_file.error_code = TEXT_MEMORY_ERROR;
        return text_file;
    }

    return text_file;
}


void GetLines(Text_file_t* p_text_file) {
    assert(p_text_file != NULL);

    size_t line_size = 0;
    for (char* i_ptr = p_text_file->buffer; *i_ptr != '\0'; i_ptr++) {
        line_size++;
        if ((*i_ptr == '\n') || (*(i_ptr + 1) == '\0')) {       // NOTE (*(i_ptr + 1) == '\0') is there for case when last line don't has \n
            if (line_size > p_text_file->longest_line_size)
                p_text_file->longest_line_size = line_size;
            line_size = 0;

            (p_text_file->lines_count)++;
        }
    }
}


Index_err_t GetIndex(Text_file_t* p_text_file) {
    assert(p_text_file != NULL);

    p_text_file->index = (Index_t)calloc(p_text_file->lines_count, sizeof(Line_pos));
    if (p_text_file->index == NULL) {
        Print_error_message("Failed to allocate memory", "p_text_file->index");
        free(p_text_file->buffer);
        return INDEX_MEMORY_ERROR;
    }

    Index_t in_index_pos = p_text_file->index;
    char* last_line_start = p_text_file->buffer;
    for (char* i_ptr = p_text_file->buffer + 1; *i_ptr != '\0'; i_ptr++) {
        if ((*i_ptr == '\n') || (*(i_ptr + 1) == '\0')) {   // NOTE (*(i_ptr + 1) == '\0') is there for case when last line don't has \n
            in_index_pos->line_start = last_line_start;
            in_index_pos->line_end = i_ptr;

            in_index_pos++;
            last_line_start = i_ptr + 1;
            *i_ptr = '\0';
        }
    }

    return INDEX_NORMAL;
}


Create_err_t CreateIndexes(Index_t* p_first_letters_index, Index_t* p_last_letters_index,
                           Text_file_t* p_text_file) {
    assert(p_first_letters_index != NULL); assert(p_last_letters_index != NULL);

    *p_first_letters_index = (Index_t)calloc(p_text_file->lines_count, sizeof(Line_pos));
    if (p_first_letters_index == NULL) {
        Print_error_message("Failed to allocate memory", "first_letters_index");
        return CREATE_MEMORY_ERROR;
    }
    *p_last_letters_index = (Index_t)calloc(p_text_file->lines_count, sizeof(Line_pos));
    if (p_last_letters_index == NULL) {
        Print_error_message("Failed to allocate memory", "last_letters_index");
        free(*p_first_letters_index);
        return CREATE_MEMORY_ERROR;
    }

    WriteIn(*p_first_letters_index, p_text_file->index, p_text_file->lines_count * sizeof(Line_pos));
    WriteIn(*p_last_letters_index, p_text_file->index, p_text_file->lines_count * sizeof(Line_pos));

    return CREATE_NORMAL;
}


int FirstLettersAlphabetCmp(const void* p_line1, const void* p_line2) {
    assert(p_line1 != NULL); assert(p_line1 != NULL);

    const Line_pos* line1_pos = (Line_pos*)p_line1;
    const Line_pos* line2_pos = (Line_pos*)p_line2;

    const char* in_str1_pos = line1_pos->line_start;
    const char* in_str2_pos = line2_pos->line_start;

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


int LastLettersAlphabetCmp(const void* p_line1, const void* p_line2) {
    assert(p_line1 != NULL); assert(p_line2 != NULL);

    const char* start_str1_pos = ((Line_pos*)p_line1)->line_start;
    const char* start_str2_pos = ((Line_pos*)p_line2)->line_start;
    const char* in_str1_pos = ((Line_pos*)p_line1)->line_end;
    const char* in_str2_pos = ((Line_pos*)p_line2)->line_end;

    while ((in_str1_pos >= start_str1_pos) && (in_str2_pos >= start_str2_pos)) {
        for (; !isalpha(*in_str1_pos) && (in_str1_pos > start_str1_pos); in_str1_pos--);
        for (; !isalpha(*in_str2_pos) && (in_str2_pos > start_str2_pos); in_str2_pos--);

        if (tolower(*in_str1_pos) > tolower(*in_str2_pos))
            return 1;
        if (tolower(*in_str1_pos) < tolower(*in_str2_pos))
            return -1;

        in_str1_pos--;
        in_str2_pos--;
    }

    if (in_str1_pos >= start_str1_pos)
        return 1;
    else if (in_str2_pos >= start_str2_pos)
        return -1;
    else
        return 0;
}


Print_err_t PrintResult(Text_file_t* p_text_file, Index_t first_letters_index, Index_t last_letters_index,
                 const char* output_file_name) {
    assert(p_text_file != NULL); assert(first_letters_index != NULL);
    assert(last_letters_index != NULL); assert(output_file_name != NULL);

    int output_file_descriptor = open(output_file_name, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (output_file_descriptor == -1) {
        Print_error_message("Failed to open with mode O_WRONLY | O_CREAT | O_TRUNC", output_file_name);
        return PRINT_OPEN_ERROR;
    }

    int stdout_descriptor = dup(STDOUT_FILENO);
    if (dup2(output_file_descriptor, STDOUT_FILENO)) {
        Print_error_message("Function returned error", "dup2");
        close(output_file_descriptor);
        return PRINT_OPEN_ERROR;
    }

    for(int i = 0; i < p_text_file->lines_count; i++)
        printf("%*s\t%*s\t%*s\n", -p_text_file->longest_line_size, first_letters_index[i].line_start,
                                  -p_text_file->longest_line_size, last_letters_index[i].line_start,
                                  -p_text_file->longest_line_size, p_text_file->index[i].line_start);
    fflush(stdout);

    dup2(stdout_descriptor, STDOUT_FILENO);
    close(output_file_descriptor);
    return PRINT_NORMAL;
}


void DestructTextFile(Text_file_t* p_text_file) {
    assert(p_text_file != NULL);

    free(p_text_file->buffer);
    free(p_text_file->index);
}


void Print_error_message(const char* message, const char* object) {
    assert(message != NULL); assert(object != NULL);

    fprintf(stderr, "%s(): %s - %s, line %d\n", __func__, message, object, __LINE__);
}


/*Index_t NonsenseGenerator (Index_t rhymed_index, int rhymed lines count,
                           Rhyme_t rhyme_type, int nonsense_quatrain_count) {
    assert(rhymed_index != NULL);

    srand(time(NULL));

    Index_t nonsense = (Index_t)calloc(nonsense_quatrain_count * 4, sizeof(char*));

    for (Index_t i_quatrain_ptr = nonsense; (i_quatrain_ptr - nonsense) < nonsense_quatrain_count * 4;
         i_quatrain_ptr += 4) {
        char* line1 = RandomEvenNumberedLine(rhymed lines count) //(((rand() % rhymed lines count) >> 1) << 1);
        char* line2 = RandomEvenNumberedLine(rhymed lines count) //(((rand() % rhymed lines count) >> 1) << 1);

        switch(rhyme_type) {
            case PAIRWISE:
                i_quatrain_ptr[0] = line1
                i_quatrain_ptr[1] = line1 + 1  // TODO MakeQuatrain()
                i_quatrain_ptr[2] = line2
                i_quatrain_ptr[3] = line2 + 1
                break;
            case CROSS:
                MakeQuatrain(line1)
                break;
            case NESTED:

                break;
        }
    }
}*/
