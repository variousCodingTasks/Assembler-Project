#include "error_handler.h"

/*
 * This module handles errors that might occur while processing
 * the input file. by "handle" I mean usually printing the error
 * to stderr in order to notify the user, while the file processing
 * modules are responsible for implementing the logic that handles
 * the error. an error will usually cause the program to not produce
 * an output, while a warning is issued only to notify a user of some
 * issues that might cause undefined behavior, but will allow the output
 * to be produced.
 */

 /*
  * this struct is used only in this module, it represents an error
  * which has a code and text.
  */
typedef struct error_record {
    int code : 8;
    char *text;
} error_record;

/* an array of "error_record" which is visible only to this file. */
static const error_record errors_list[] = {
    {0, " "},
    {1, "is an illegal label."},
    {2, "such symbol already exists."},
    {3, "is an unknown command."},
    {4, "is missing a colon \":\"."},
    {5, "operands should be separated with commas."},
    {6, "excessive text at the end of line."},
    {7, "too few parameters."},
    {8, "incorrect input operand type."},
    {9, "incorrect output operand type."},
    {10, "is not a legal number."},
    {11, "label name has too many characters."},
    {12, "label name should start with a letter."},
    {13, "label contains illegal characters."},
    {14, "missing comma between numbers."},
    {15, "illegal character."},
    {16, "excessive comma at the end of numbers list."},
    {17, "string definition is missing opening double quotes."},
    {18, "string definition is missing closing double quotes."},
    {19, "excessive text following string definition."},
    {20, "illegal character detected."},
    {21, "empty structure definition."},
    {22, "missing number definition."},
    {23, "number in structure definition should be followed by a comma."},
    {24, "entry/extern declaration error: such label already exists."},
    {25, "numbers list is empty."},
    {26, "undeclared variable."},
    {27, "operand is not of data/extern type."},
    {28, "operand is not a structure."},
    {29, "the specified label does not exist."},
    {30, "the specified label is not of data type."},
    {31, "the label was followed by empty text."},
    {32, "the string definition is empty."}
};

/* an array "error_record" that contains warnings. */
static const error_record warnings_list[] = {
    {0, " "},
    {1, "value will not fit in 8 bits."},
    {2, "line opening label will be ignored with \".entry\"/\".extern\" declarations."},    {3, "value will not fit in 10 bits."}
};

/*
 * print_error_string:
 * prints "str" and the error message that corresponds to "error_number" in
 * the "errors_list", the message will also include the line number "line_count".
 */
void print_error_string(int line_count, int error_number, char *str){
    fprintf(stderr, "Error, line %d: \"%s\" %s\n", line_count, str, errors_list[error_number].text);
}

/*
 * print_error_char:
 * prints "c" and the error message that corresponds to "error_number" in
 * the "errors_list", the message will also include the line number "line_count".
 */
void print_error_char(int line_count, int error_number, char c){
    fprintf(stderr, "Error, line %d: \'%c\' %s\n", line_count, c, errors_list[error_number].text);
}

/*
 * print_error:
 * prints the error message that corresponds to "error_number" in
 * the "errors_list", the message will also include the line number "line_count".
 */
void print_error(int line_count, int error_number){
    fprintf(stderr, "Error, line %d: %s\n", line_count, errors_list[error_number].text);
}

/*
 * print_warning_string:
 * prints "str" and the warning message that corresponds to "warning_number" in
 * the "warnings_list", the message will also include the line number "line_count".
 */
void print_warning_string(int line_count, int warning_number, char *str){
    fprintf(stderr, "Warning, line %d: \"%s\" %s\n", line_count, str, warnings_list[warning_number].text);
}

/*
 * print_warning_int:
 * prints "value" and the warning message that corresponds to "warning_number" in
 * the "warnings_list", the message will also include the line number "line_count".
 */
void print_warning_int(int line_count, int warning_number, int value){
    fprintf(stderr, "Warning, line %d: \"%d\" %s\n", line_count, value, warnings_list[warning_number].text);
}

/*
 * print_warning:
 * prints the warning message that corresponds to "warning_number" in
 * the "warnings_list", the message will also include the line number "line_count".
 */
void print_warning(int line_count, int warning_number){
    fprintf(stderr, "Warning, line %d: %s\n", line_count, warnings_list[warning_number].text);
}

/*
 * exit_program_fatal_error:
 * exits the program when a fatal error occurs, usually when the environment is unable
 * to allocate new memory. the reason this function has a return type is that
 * some of the functions which might call it have one themselves, so in order
 * to avoid a warning printed by the compiler a return type has been added, so
 * the calling functions can use it.
 */
void *exit_program_fatal_error(void){
    fprintf(stderr, "Fatal Error: unable to allocate memory, exiting program!\n\n");
    exit(EXIT_FAILURE);
    return NULL;
}