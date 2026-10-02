#include <stdio.h>
#include <stdlib.h>
#include <assert.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <ctype.h>
#include <time.h> // TODO add to readme DONE
#include "qsort.c"

#define TEXT_FILE_NAME "onegin.txt"
#define OUTPUT_FILE_NAME "output.txt"

#define DEFAULT_NONSENSE_QUATRAIN_COUNT 2  // TODO add nonsense generator

enum {MAIN_NORMAL = 0, MAIN_TEXT_ERROR = 1, MAIN_CREATE_ERROR = 2,
      MAIN_PRINT_ERROR = 3, MAIN_NONSENSE_ERROR = 4};                     // return of main()
typedef enum {TEXT_NORMAL = 0, TEXT_MEMORY_ERROR = 1,
              TEXT_OPEN_ERROR = 2, TEXT_READ_ERROR = 3}   Text_err_t;     // field of structure returned by GetIndex()
typedef enum {INDEX_NORMAL = 0, INDEX_MEMORY_ERROR = 1}   Index_err_t;    // return of GetIndex()
typedef enum {CREATE_NORMAL = 0, CREATE_MEMORY_ERROR = 1} Create_err_t;   // return of CreateIndexes()
typedef enum {PRINT_NORMAL = 0, PRINT_OPEN_ERROR = 2}     Print_err_t;    // return of PrintResult()
typedef enum {NONSENSE_NORMAL = 0, NONSENSE_MEMORY_ERROR = 2,
              NONSENSE_INPUT_DATA_ERROR = 3}              Nonsense_err_t; // return of PrintNonsense()
typedef enum {GENERATE_NORMAL = 0, GENERATE_MEMORY_ERROR = 2,
              GENERATE_INPUT_DATA_ERROR = 3}              Generate_err_t; // field of structure returned by GenerateNonsense()

struct Nonsense_t {char** nonsense; Generate_err_t gen_err;};             // return of GenerateNonsense()
// TODO return of generator
typedef enum {PAIRWISE, CROSS, NESTED} Rhyme_t;

struct Flags {
    unsigned int nonsense_flag : 1;
    int nonsense_flag_arg = DEFAULT_NONSENSE_QUATRAIN_COUNT;
};

struct Line_pos {char* line_start; char* line_end;}; // TODO array of struct DONE

typedef Line_pos* Index_t;

struct Text_file_t {ssize_t buffer_size; char* buffer; size_t text_size;
                    int lines_count; size_t longest_line_size; Index_t index;
                    Text_err_t error_code;};  // TODO remove exits DONE

/**
 * @brief free some fields of Text_file_t structure
 *
 * @param[in] p_text_file pointer to Text_file_t structure
 */
void DestructTextFile (Text_file_t* p_text_file);

/**
 * @brief print error message
 *
 * @param[in] message error message
 * @param[in] object  object of error
 */
#define Print_error_message(message, object)                                         \
    fprintf(stderr, "%s(): %s - %s, line %d\n", __func__, message, object, __LINE__);
// TODO macro for errors DONE

/**
 * @brief get command line flags with their arguments from command line arguments
 *
 * @param[in]  argc argc from command line
 * @param[in]  agrv argv from command line
 * @param[out] command_line_flags pointer to stract with flags
 */
void HandleArg(int argc, char* argv[], Flags* command_line_flags);

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
 * @brief fill quatrain at specified addres
 *
 * @param[out] quatrain_ptr   pointer to start of quatrain
 * @param[in]  line_index_1st first line of quatrain
 * @param[in]  line_index_2nd second line of quatrain
 * @param[in]  line_index_3rd third line of quatrain
 * @param[in]  line_index_4th fourth line of quatrain
 */
void MakeQuatrain(Index_t rhymed_index, char** quatrain_ptr,
                  size_t line_index_1st, size_t line_index_2nd,
                  size_t line_index_3rd, size_t line_index_4th);

/**
 * @brief random generating of rhymed nonsense
 *
 * @param[in] rhymed_index            rhymed index array
 * @param[in] rhymed_lines_count      count of lines in rhymed index array
 * @param[in] rhyme_type              type of rhyme (PAIRWISE, CROSS, NESTED)
 * @param[in] nonsense_quatrain_count count of being printed quatrain of nonsense
 *
 * @return generated array of lines
 */
Nonsense_t GenerateNonsense(Index_t rhymed_index, int rhymed_lines_count,
                           Rhyme_t rhyme_type, int nonsense_quatrain_count); // TODO add to readme

/**
 * @brief print random generated rhymed nonsense
 *
 * @param[in] rhymed_index            rhymed index array
 * @param[in] rhymed_lines_count      count of lines in rhymed index array
 * @param[in] rhyme_type              type of rhyme (PAIRWISE, CROSS, NESTED)
 * @param[in] nonsense_quatrain_count count of being printed quatrain of nonsense
 *
 * @return NONSENSE_NORMAL or error number
 */
Nonsense_err_t PrintNonsense(Index_t rhymed_index, int rhymed_lines_count,
                             Rhyme_t rhyme_type, int nonsense_quatrain_count);


int main(int argc, char* argv[]) {
    Flags command_line_flags = {};
    HandleArg(argc, argv, &command_line_flags);

    Text_file_t text_file = GetText(TEXT_FILE_NAME);
    if (text_file.error_code != TEXT_NORMAL)
        return MAIN_TEXT_ERROR;

    Index_t first_letters_index = NULL;
    Index_t last_letters_index = NULL;
    if (CreateIndexes(&first_letters_index, &last_letters_index, &text_file) != CREATE_NORMAL) { // TODO creat func DONE
        DestructTextFile(&text_file);
        return MAIN_CREATE_ERROR;
    }

    MYqsort(first_letters_index, first_letters_index + text_file.lines_count - 1, sizeof(Line_pos),
            UpgradedHoarePartition, FirstLettersAlphabetCmp);
    qsort(last_letters_index, text_file.lines_count, sizeof(Line_pos), LastLettersAlphabetCmp);

    if (PrintResult(&text_file, first_letters_index, last_letters_index, OUTPUT_FILE_NAME) != PRINT_NORMAL) {
        DestructTextFile(&text_file); free(first_letters_index); free(last_letters_index); // TODO destruct DONE
        return MAIN_PRINT_ERROR;
    }

    if (command_line_flags.nonsense_flag) { // TODO one func at main for nonsense
        if (PrintNonsense(last_letters_index, text_file.lines_count,
                          CROSS, command_line_flags.nonsense_flag_arg) != NONSENSE_NORMAL) {
            DestructTextFile(&text_file); free(first_letters_index); free(last_letters_index);
            return MAIN_NONSENSE_ERROR;
        }
    }

    DestructTextFile(&text_file); free(first_letters_index); free(last_letters_index);
    return MAIN_NORMAL;
}


void HandleArg(int argc, char* argv[], Flags* command_line_flags) {  // TODO clf
    assert(argv != NULL); assert(command_line_flags != NULL);

    for (int arg_index = 1; arg_index < argc; arg_index++) {
        const char* arg_position = argv[arg_index];
        if (*arg_position++ != '-')
            continue;

        while (isalpha(*arg_position)) {
            switch (*arg_position) {
                case 'n':
                    command_line_flags->nonsense_flag = 1;
                    if ((sscanf(++arg_position, "%d", &(command_line_flags->nonsense_flag_arg)) != 1) && (arg_index + 1) < argc)
                        sscanf(argv[arg_index + 1], "%d", &(command_line_flags->nonsense_flag_arg));
                    if (command_line_flags->nonsense_flag_arg < 0)
                        command_line_flags->nonsense_flag_arg = DEFAULT_NONSENSE_QUATRAIN_COUNT;
                default: break;
            }

            arg_position++;
        }
    }
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

    close(file_descriptor);    // TODO 2 func for lines and index DONE

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


void DestructTextFile(Text_file_t* p_text_file) {
    assert(p_text_file != NULL);

    free(p_text_file->buffer);
    free(p_text_file->index);
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
    const char* start_str2_pos = ((Line_pos*)p_line2)->line_start; // TODO remember start DONE
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

// TODO fprintf DONE
Print_err_t PrintResult(Text_file_t* p_text_file, Index_t first_letters_index, Index_t last_letters_index,
                 const char* output_file_name) {
    assert(p_text_file != NULL); assert(first_letters_index != NULL);
    assert(last_letters_index != NULL); assert(output_file_name != NULL);

    FILE* output_file = fopen(output_file_name, "w");
    if (output_file == NULL) {
        Print_error_message("Failed to open with mode \"w\"", output_file_name);
        return PRINT_OPEN_ERROR;
    }

    for(int i = 0; i < p_text_file->lines_count; i++)  // TODO use longest line size
        fprintf(output_file, "%*s\t%*s\t%*s\n",
                            -p_text_file->longest_line_size, first_letters_index[i].line_start,
                            -p_text_file->longest_line_size, last_letters_index[i].line_start,
                            -p_text_file->longest_line_size, p_text_file->index[i].line_start);

    fclose(output_file);
    return PRINT_NORMAL;
}

// TODO MakeQuatrain()
void MakeQuatrain(Index_t rhymed_index, char** quatrain_ptr,
                  size_t line_index_1st, size_t line_index_2nd,
                  size_t line_index_3rd, size_t line_index_4th) {
    quatrain_ptr[0] = rhymed_index[line_index_1st].line_start;
    quatrain_ptr[1] = rhymed_index[line_index_2nd].line_start;
    quatrain_ptr[2] = rhymed_index[line_index_3rd].line_start;
    quatrain_ptr[3] = rhymed_index[line_index_4th].line_start;
}
// TODO rhyme type

#define RandomEvenNumberedLineIndex(rhymed_lines_count) (1 + (((rand() % (rhymed_lines_count - 1)) >> 1) << 1))

Nonsense_t GenerateNonsense(Index_t rhymed_index, int rhymed_lines_count,
                         Rhyme_t rhyme_type, int nonsense_quatrain_count) {
    assert(rhymed_index != NULL);

    Nonsense_t nonsense = {};

    if (rhymed_lines_count < 2) {
        Print_error_message("To few lines for generating nonsense", "rhymed_lines_count");
        nonsense.gen_err = GENERATE_INPUT_DATA_ERROR;
        return nonsense;
    }

    nonsense.nonsense = (char**)calloc(nonsense_quatrain_count * 4, sizeof(char*));
    if (nonsense.nonsense == NULL) {
        Print_error_message("Failed to allocate memory", "nonsense");
        nonsense.gen_err = GENERATE_MEMORY_ERROR;
        return nonsense;
    }
    size_t line1_index = 0;
    size_t line2_index = 0;

    srand(time(NULL));

    for (char** i_quatrain_ptr = nonsense.nonsense;
         (i_quatrain_ptr - nonsense.nonsense) < nonsense_quatrain_count * 4; i_quatrain_ptr += 4) {
        line1_index = RandomEvenNumberedLineIndex(rhymed_lines_count);
        line2_index = RandomEvenNumberedLineIndex(rhymed_lines_count);

        switch(rhyme_type) {
            case PAIRWISE:
                MakeQuatrain(rhymed_index, i_quatrain_ptr,
                             line1_index, line1_index - 1, line2_index, line2_index - 1);
                break;
            case CROSS:
                MakeQuatrain(rhymed_index, i_quatrain_ptr,
                             line1_index, line2_index, line1_index - 1, line2_index - 1);
                break;
            case NESTED:
                MakeQuatrain(rhymed_index, i_quatrain_ptr,
                             line1_index, line2_index, line2_index - 1, line1_index - 1);
                break;
            default:
                break;
        }
    }

    return nonsense;
}


Nonsense_err_t PrintNonsense(Index_t rhymed_index, int rhymed_lines_count,
                             Rhyme_t rhyme_type, int nonsense_quatrain_count) {
    Nonsense_t nonsense = GenerateNonsense(rhymed_index, rhymed_lines_count,
                                        rhyme_type, nonsense_quatrain_count);
    if (nonsense.gen_err == GENERATE_INPUT_DATA_ERROR)
        return NONSENSE_INPUT_DATA_ERROR;
    if (nonsense.gen_err == GENERATE_MEMORY_ERROR)
        return NONSENSE_MEMORY_ERROR;

    for (int quatrain_number = 0; quatrain_number < nonsense_quatrain_count; quatrain_number++) {
        for (int line_number = 0; line_number < 4; line_number++)
            printf("%s\n", nonsense.nonsense[quatrain_number * 4 + line_number]);
        printf("\n");
    }

    free(nonsense.nonsense);
    return NONSENSE_NORMAL;
}
