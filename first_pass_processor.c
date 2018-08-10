#include "first_pass_processor.h"
 
/*
 * this module is responsible for processing the input file in the first pass.
 * the main purpose is to translate the different lines, depending on their types,
 * to words stored in the instructions and data arrays of the memory manager.
 * at the end of this phase, the arrays should include all the data required for
 * the imaginary processor to run the program, apart from the addresses of the
 * "ABSOLUTE" type variables passed as operands, this part is left for the second
 * pass processor.
 * each line of the file is processed by first detecting its type: a comment or
 * a blank line is skipped, then preprocessing phase determines if the line belongs
 * to an instruction or a directive, and if it includes a label. the proper function
 * is called to process the rest of the line, resulting in a chain of function calls
 * (described below) which read, process, store, error check and report the different
 * labels, operands and commands (directives and instructions). the functions called
 * store the word values that represent the instructions, their encoding and the addresses
 * of the operands detectable at this stage (like registers, struct fields and immediate 
 * values), ABSOLUTE types have 0 stored as their addresses but their spot in the instructions
 * array is reserved for second pass processing. the functions also update the lists
 * for the second pass processor by saving references each time an operand needs to be
 * revisited in order to have the proper address set in the instructions array. they
 * also add references for the lists used to produce the entries and externs files.
 * if a chain of functions does not detect any errors, they will process the whole
 * line stopping at the beginning of the next one, otherwise line is skipped by the
 * main line processing function. a line counter is kept global to make it easier
 * for error reporting functions to read it. the line processing stops when the first
 * error of the specific line is detected, this is to notify the user and direct their
 * attention to the faulty lines. once an error is detected, it does not make any sense
 * to try and find additional errors since the line's composition becomes unpredictable 
 * and will not follow a predefined logic, therefore, the main focus should be to notify
 * the user about these lines, rather than trying to find each and every error present.
 * the processor, however, will continue processing the lines, even if one has been
 * detected to contain errors.
 * if at any point the memory becomes full, the program will still process the next
 * lines, however, no new words will be saved, and no output files will be produced
 * since the program might cause errors when the imaginary CPU tries to run it.
 */

 /*
  * "intput_as": is a pointer to a file accessible to all the functions in this
  * file, which represents the input file for the assembler.
  * "line_count" represents the line counter, the reason it's set as a global
  * variable in this file, is to allow easy access for error reporting functions,
  * instead of passing it down a function chain, making the code less readable.
  */
static FILE *intput_as = NULL;
static int line_count = 0;

/*
 * load_input_file:
 * opens the ".as" input file named "filename", initializes line_count to 0.
 * returns a pointer to a FILE, where all input is directed for the functions
 * in this module. should be called when starting to process a new file.
 */
FILE *load_input_file(char *filename){
   intput_as = fopen(filename, "r");
   line_count = 0;
   return intput_as;
}

/*
 * close_input_file:
 * should be called when processing is done to close the file an set its value
 * to NULL, in case another file needs to be processed.
 */
void close_input_file(void){
    fclose(intput_as);
    intput_as = NULL;
}

/*
 * skip_whites:
 * skips spaces and tabs, returns the first non space nor tab char
 * it reads to the caller and to the file "intput_as".
 */
static int skip_whites(void){
    int c;
    while((c = getc(intput_as)) == '\t' || c == ' ')
        ;
    ungetc(c, intput_as);
    return c;
}

/*
 * skip_line:
 * it skips and consumes all the characters until a line break or EOF is
 * detected (and put back into the input file).
 */
static int skip_line(void){
    int c;
    while((c = getc(intput_as)) != '\n' && c != EOF)
        ;
    return c;
}

/*
 * peek_next_char:
 * skips spaces and tabs, then reads the next char, returns it to the
 * user and the input file.
 */
static int peek_next_char(void){
    int c;
    skip_whites();
    c = getc(intput_as);
    ungetc(c, intput_as);
    return c;
}

/*
 * read_next_string:
 * tries to read the next string and returns the number of characters it has read
 * through, the output is saved in "string". should work like "fscanf" that skips
 * spaces and tabs before it starts scanning, but this one stops when a colon is
 * reached as well (the colon is not returned to the input file once consumed). this
 * is used to read labels, commands and operands. when reading an operand that contains
 * a colon the function is still correct since an error will be detected anyway,
 * for a colon is not allowed in operands names anyway. this function will not read
 * through a linebreak.
 * this function is used to read the operands of ".entry"/".extern" directives, instructions
 * have their own operand processing functions.
 */
static int read_next_string(char *string){
    int c, chars_count;
    char *p = string;
    while((c = getc(intput_as)) != EOF && c != ':' && c != '\n' && c != ' ' && c != '\t'){
        chars_count++;
        *p++ = c;
    }
    if (c == ':') *p++ = c;
    else ungetc(c, intput_as);
    *p = '\0';
    skip_whites();
    return chars_count;
}

/*
 * this is an alternative version of "read_next_string", which I decided not to use.
 * this version does not tolerate situations where no space appears after the string
 * the program is trying to scan for, since it uses "fscanf". it will not read to the
 * next line however. where this one will result in an error when reading through: 
 * "LABEL:.data..."since it reads as one word, the function above will stop at the colon
 * character.
 * **I added this following a discussion in the course's forum.
 * 
static int read_next_string(char *string){
    int item_read = 0;
    if (peek_next_char() == '\n')
        *string = '\0';
    else {
        item_read = fscanf(intput_as,"%s",string);
        skip_whites();        
    }
    return item_read;
}
 */

/*
 * read_next_number:
 * reads the next number (as a string) and saves it in "dest". returns
 * the status which indicates if a number has been read. the function
 * will tolerate '+' and '-' only if the appear on the left of a number,
 * if they appear alone (or with a trailing space) they are not considered
 * a number, this why "atoi" is not used instead. the function returns any
 * non digit character it has read through last to the input file.
 */
static int read_next_number(int *dest){
    int sign = 1, current_number = 0, status = 0;
    int c = getc(intput_as), next_c = getc(intput_as);
    ungetc(next_c, intput_as);
    if (c == '+' && isdigit(next_c)) sign = 1;
    else if (c == '-' && isdigit(next_c)) sign = -1;
    else if(!isdigit(c)){
        ungetc(c, intput_as);
        return status;
    }
    else
        ungetc(c, intput_as);
    while(isdigit(c = getc(intput_as))){
        current_number = 10 * current_number + (c - '0');
        status++;
    }	
    ungetc(c, intput_as);
    *dest = sign * current_number;
    return status;
}

/*
 * shorten_string:
 * assumes "string" has at least 9 cells, which wont be a problem since
 * it will always be called on strings which have "MAX_BUFFER_SIZE" cells.
 * this functions shortens the string to its first 5 characters and adds
 * "..." to its end, this should be called when a label is too long to be
 * fully printed to some output (stderr in this case).
 */
static char *shorten_string(char *string){
    string[5] = '\0';
    strcat(string , "...");
    return string;
}

/*
 * is_legal_label:
 * checks if "label" is legal and returns an integer indicating this.
 * "is_new_label" indicates whether the label should end with a colon,
 * for labels that appear at the beginning of the line. a legal label:
 * starts with a letter, contains only numbers and digits, shorter than
 * 30 characters (excluding '\0'). 0 is returned in case the label is
 * not legal.
 */
static int is_legal_label(char *label, int is_new_label){
    int i, length = strlen(label);
    if (!isalpha(label[0])){
        print_error_string(line_count, 12, label);
        return 0;
    }
    if (is_new_label && (label[length - 1] != ':')){
        print_error_string(line_count, 4, label);
        return 0;
    }
    if ((!is_new_label && length > 30) || (is_new_label && length > 31)){
        print_error_string(line_count, 11, shorten_string(label));
        return 0;        
    }
    for(i = 1; i < length - 1 ; i++)
        if(!isalpha(label[i]) && !isdigit(label[i])){
            print_error_string(line_count, 13, label);
            return 0;
    }
    if (!is_new_label && length > 1 && !isalpha(label[length -1]) && !isdigit(label[length - 1])){
        print_error_string(line_count, 13, label);
        return 0;
    }
    return 1;
}

/*
 * remove_colon:
 * removes the colon at the end of the string (the one before the terminating
 * '\0') and returns a pointer to it. it assumes that there is a colon at the
 * end of the string, and should not be called on it otherwise.
 */
static char *remove_colon(char *src){
    char *p = src;
    while (*p != ':')
        p++;
    *p ='\0';
    return src;
}

/*
 * is_comment_or_empty_line:
 * checks if a line is a comment (first non tab/space character is ';')
 * or empty (next character is line break), returns 1 if so, 0 otherwise.
 */
static int is_comment_or_empty_line(void){
    int c = skip_whites();
    if (c == '\n' || c == ';'){
        return 1;   
    }
    else
        return 0;
}

/*
 * is_command:
 * checks if the "str" is a key for a node in the symbols table, and stores
 * a pointer to it in "symbol", or NULL if not found. if the node belongs to
 * an instruction ("mov", "lea", etc..) or a directive (e.g. ".data"),
 * 1 is returned, 0 otherwise.
 */
static int is_command(char *str, node **symbol){
    *symbol = find_symbol(str);
    if (*symbol && ((*symbol)->type == INST || (*symbol)->type == DIRECT))
        return 1;
    return 0;
}

/*
 * pre_process_line_error_check:
 * checks and reports the relevant error which "pre_process_line" has encountered
 * and returned a status of 0. the input parameters are set by the calling function.
 */
static void pre_process_line_error_check(char *str1, char *str2, int str2_is_command, node **symbol, int str1_legal_label){
    if (*symbol)
        print_error_string(line_count, 2, str1);
    else if (str1_legal_label && !strcmp("", str2))
        print_error_string(line_count, 31, str1);
    else if (!str2_is_command && str1_legal_label)
        print_error_string(line_count, 3, str2);
}

/*
 * pre_process_line:
 * reads the first string in the line (after skipping tabs and spaces), by calling
 * "read_next_string" which stores the result in "str1": if it has read a command,
 * then the line is treated like no label is present, otherwise, the next string is
 * read and stored in "str2": if "str1" is a legal label (which ends with a colon)
 * and "str2" is a legal command, then 1 is returned (for success), 0 other wise.
 * the "label_flag" indicates if a label is present, "command" stores any command
 * name (whether it's the first or second string in the line), "label" stores the
 * label excluding the colon at its end ("remove_colon" is called on it) and "symbol"
 * stores the node of the command from the symbols table. "is_legal_label" is called
 * to check if the label is legal and has a colon as its end as well.
 */
static int pre_process_line(char *label, char *command, int *label_flag, node **symbol){
    int str1_legal_label, str2_is_command;
    char str1[MAX_BUFFER_SIZE] = "", str2[MAX_BUFFER_SIZE] = "";
    read_next_string(str1);
    if (is_command(str1, symbol)){
        strcpy(command, str1);
        return 1;
    }
    else if ((str1_legal_label = is_legal_label(str1, 1)) && !(*symbol = find_symbol(remove_colon(str1)))){
        read_next_string(str2);
        if ((str2_is_command = is_command(str2, symbol))){
            strcpy(label, str1);
            strcpy(command, str2);
            *label_flag = 1;
            return 1;
      }
    }
    pre_process_line_error_check(str1, str2, str2_is_command, symbol, str1_legal_label);
    return 0;
}

/*
 * read_numbers_list_error_check:
 * checks and reports any errors detected by "read_numbers_list" and 
 * takes parameters which were read by the caller. the function reports
 * only one error depending on the parameters supplied. "numbers_read"
 * indicates if any numbers were read by "read_numbers_list" and "c" is
 * the last character it has went through.
 */
static void read_numbers_list_error_check(char c, int numbers_read){
    if (!numbers_read && (c == '\n' || c == ','))
        print_error(line_count, 25);
    else if (c == '\n')
        print_error(line_count, 16);
    else if (isdigit(c))
        print_error(line_count, 14);
    else if ((c == '+' || c == '-')){
        getc(intput_as);
        if (isdigit(peek_next_char()))
            print_error(line_count, 14);
        else
            print_error_char(line_count, 15, c);
        ungetc(c, intput_as);
    }
    else if (!isdigit(c))
        print_error_char(line_count, 15, c);
}

/*
 * read_numbers_list:
 * should read the next numbers list and stores them into the data array of
 * the memory manager, as long as no errors are detected: if an error is detected
 * it stops reading, however it wont remove the item it has already read from
 * the data array, since it will not be saved to a file anyway. the numbers
 * should be separated by commas and be the last thing to a appear on the line,
 * no trailing text should appear afterwards, otherwise it's an error. if the 
 * number read wont fit in 10 bits (assuming it's signed), then a warning is
 * printed, since this could cause undefined behavior when the resulting program 
 * runs. if any error is detected, this function returns 0 and calls its error
 * checking function.
 * 
 */
static int read_numbers_list(void){
    int c, number, numbers_read = 0;
    word temp_word;
    while (read_next_number(&number)){
        numbers_read++;
        if (number > 511 || number < -512) print_warning_int(line_count, 3, number);
        temp_word.value = number;
        data_array_insert(temp_word);
        if ((c = peek_next_char()) == '\n' || c == EOF){
            c = getc(intput_as);
            return numbers_read;
        }
        else if (c == ','){
            c = getc(intput_as);
            skip_whites();
        }
        else break;
    }
    read_numbers_list_error_check(peek_next_char(), numbers_read);
    return 0;
}

/*
 * read_string_error_check:
 * does the error reporting for "read_string_error_check" and receives parameters
 * read by the caller. the parameters are flags indicating if a certain character
 * is detected at a given position in the line.
 */
static void read_string_error_check(char c, int openning_quotes_flag, int closing_quotes_flag, int excessive_text_flag){
    if (!openning_quotes_flag && (c == '\n' || c == EOF)){
        ungetc(c, intput_as);
        print_error(line_count, 32); 
    }
    else if (!openning_quotes_flag)
        print_error(line_count, 17);
    else if (!closing_quotes_flag)
        print_error(line_count, 18);
    else if (excessive_text_flag)
        print_error(line_count, 19);
}

/*
 * read_string:
 * reads the next string definition: used when reading the string part of a 
 * struct or a string data type, either way, should be the last thing to read
 * in a line. a proper string definition starts and ends with double quotes mark,
 * so both need to be detected for the input to be correct, no trailing text allowed.
 * each character is inserted in the data array of the memory manager and 0
 * is inserted at the end of the section. if any errors detected (like a trailing text
 * or a missing closing double quotes mark) the characters read wont be removed
 * from the data array, since no output will be produced anyway. if any errors are
 * detected" 0 is returned and error checking function is called.
 */
static int read_string(void){
    int c, excessive_text_flag = 0, openning_quotes_flag = 0, closing_quotes_flag = 0;
    word temp_word;
    if ((c = getc(intput_as)) == '\"'){
        openning_quotes_flag = 1;
        while((c = getc(intput_as)) != EOF && c != '\n' && c != '\"'){
            temp_word.value = c;
            data_array_insert(temp_word);
        }
        if (c == '\n' || c == EOF) ungetc(c, intput_as);
        if (c == '\"') closing_quotes_flag = 1;
        if (c == '\"' && (((c = peek_next_char()) == '\n') || c == EOF)){
            temp_word.value = 0;
            data_array_insert(temp_word);
            skip_line();
            return 1;
        }
        else excessive_text_flag = 1;
    }
    read_string_error_check(c, openning_quotes_flag, closing_quotes_flag, excessive_text_flag);
    return 0;
}

/*
 * struct_error_check:
 * the error reporting function for "process_directive_struct", in case an error was
 * detected. "number_read_flag" indicates if the number part of the struct definition
 * has been read and "trailing_comma_flag" indicates whether the number part was
 * followed by comma, otherwise, it is an error. the error checking function for
 * the string part belongs to the string reading function.
 */
static void struct_error_check(int number_read_flag, int trailing_comma_flag){    
    if (!number_read_flag && peek_next_char() == '\n')
        print_error(line_count, 21);
    else if (!number_read_flag && !trailing_comma_flag)
        print_error(line_count, 22);
    else if (!number_read_flag && peek_next_char() != ',')
        print_error_char(line_count, 20, peek_next_char());
    else if (number_read_flag && !trailing_comma_flag)
        print_error(line_count, 23);
}

/*
 * process_directive_struct:
 * processes a directive ".struct" definition: reads the number part, then
 * reads through the trailing comma, then calls "read_string" to read the 
 * string part of the definition. if the definition does not follow the
 * syntax, 0 is returned and error reporting function is called. the "read_string"
 * is responsible for reporting any errors that might occur in the string part
 * and is the one that skips the line in case everything goes well.
 */
static int process_directive_struct(void){
    int status, number, number_read_flag = 0, trailing_comma_flag = 0;
    if ((status = number_read_flag = read_next_number(&number))){
        word temp_word;
        temp_word.value = number;
        data_array_insert(temp_word);
        if ((status = trailing_comma_flag = (peek_next_char() == ','))){
            getc(intput_as);
            skip_whites();
            status = read_string();
        }
    }
    struct_error_check(number_read_flag, trailing_comma_flag);
    return status;
}

/*
 * process_directive_ext_ent:
 * this function process processes ".entry"/".extern" directives: "is_label"
 * indicates whether a label is present at the beginning of the line, if so
 * a warning is printed (nothing is done with label, even if such already exists),
 * the label, however, must be legal. "is_ext" indicates whether ".extern" is
 * the directive being processed. the next string is then read, and should also
 * be legal, and should represent the label's name: if the symbol is extern and
 * no such symbol already exists, it is placed in the symbols table, no other
 * extern directive should contain the same symbol, neither a symbol used in entry
 * directive(entries_list_find does this check). if this is an entry directive, then
 * it is checked that no other similar entry directive already exists, nor an extern
 * label defined before, otherwise, it is an error and 0 is returned (and the error 
 * reporting function is called).
 * if no similar other entry directive exists , the label is placed in the entries_list
 * for the second pass processor.
 * the function also makes sure that no trailing text appears after the labels name.
 */
static int process_directive_ext_ent(int is_ext, int is_label){
    int status = 1;
    char label[MAX_BUFFER_SIZE];
    if (is_label) print_warning(line_count, 2);
    status = status && read_next_string(label);
    if (status && (status = is_legal_label(label, 0))){
        if (is_ext && !(find_symbol(label) || entries_list_find(label)))
            symbol_table_insert_label(label, get_dc(), EXTERN, 0);
        else if (!is_ext && !entries_list_find(label) && (!find_symbol(label) || find_symbol(label)->type != EXTERN))
            ent_ext_list_insert(label, 1, line_count);
        else {
            status = 0;
            print_error_string(line_count, 24, label);             
        }
    }
    if (!status) return status;
    if ((peek_next_char() == '\n' || peek_next_char() == EOF)) skip_line();
    else {
        status = 0;
        print_error(line_count, 6);
    }
    return status;
}

/*
 * process_directive:
 * processes a directive (a symbol with "DIRECT" value in the symbol table), and
 * is called by "process_line". "is_label" marks if a label is present at the 
 * beginning of the line, "label" should hold the label in case present and "direct"
 * is the node of the directive command from the symbol table. the function returns
 * 0 is an error was detected by any of the functions it calls or any of the functions
 * they have called. if ".entry"/".extern" were used, their processing function is 
 * called ("process_directive_ext_ent"). "is_struct" flag indicates whether ".struct"
 * is the directive being used (this is used by the second pass processor to check
 * if an operand is indeed a struct). if a label is present before the directive,
 * a symbol is inserted into the symbols table, "get_dc()" is used to mark the index
 * of the first word that belongs to the directive's section in the data table. the
 * function then calls the proper syntax processing function defined above.
 */
static int process_directive(node *direct, int is_label, char *label){
    int status = 1;
    int is_struct = strcmp(direct->key, ".struct") == 0 ? 1 : 0;
    if (!strcmp(direct->key, ".entry"))
        status = process_directive_ext_ent(0, is_label);
    else if (!strcmp(direct->key, ".extern"))
        status = process_directive_ext_ent(1, is_label);
    else {
        if (is_label)
            symbol_table_insert_label(label, get_dc(), DATA, is_struct);
        if (status && !strcmp(direct->key, ".data"))
            status = read_numbers_list();
        else if (status && !strcmp(direct->key, ".string"))
            status = read_string();
        else if (status && !strcmp(direct->key, ".struct"))
            status = process_directive_struct();
    }
    if (!status) skip_line();
    return status;
}

/*
 * detect_operand_type:
 * detects the operands label and saves it in "dest" and returns it's type as
 * it appears in the input, in order to determine its addressing type. the
 * function reads characters and copies them one by one to "dest" until it is
 * stopped by certain characters. and places '\0' at the end of "dest". it then
 * checks for different indicators to determine the operands addressing type.
 * this function will tolerate operands that don't have a space between them and
 * a trailing comma character, for example: "..op1,.." will correctly read "op"
 * and stop at the comma. if you want to be strict about a trailing space, then
 * simply remove the " c != ',' " part from the 4th line of the function, and
 * it will read "op," including the comma from the example above, which will cause
 * an error since this is not a legal operand.
 */
static int detect_operand_type(char *dest){
    int c, length;
    char *p = dest;
    while((c = getc(intput_as)) != EOF && c != '\n' && c != ',' && c != ' ' && c != '\t')
        *p++  = c;
    ungetc(c, intput_as);
    *p = '\0';
    length =strlen(dest);
    if (dest[0] == '#') return IMMEDIATE;
    else if (dest[length - 2] == '.' && (dest[length - 1] == '1' || dest[length - 1] == '2')) return STRUCT;
    else if (find_symbol(dest) && find_symbol(dest)->type == REGS) return REGISTER;
    else if (length > 0) return ABSOLUTE;    
    else return -1;
}

/*
 * detect_operands_error_check:
 * checks if the operands types "op1_type" and "op2_type" were detected successfully,
 * otherwise the line contained no operands (the default case for an operand is ABSOLUTE
 * ). also checks if a comma is present in case to operands were passed and that
 * there's no additional text at the end of the line. the third case also covers errors
 * that has to do with the user entering too many operands for a given command.
 */
static void detect_operands_error_check(int *op1_type, int *op2_type, char c, int comma_detected){
    if (*op1_type == -1 || *op2_type == -1)
        print_error(line_count, 7);
    else if (!comma_detected)
        print_error(line_count, 5);
    else if (c != '\n' && c != EOF)
        print_error(line_count, 6);
}

/*
 * detect_operands_and_types:
 * reads the operands of an instruction and sets their types by calling
 * "detect_operand_type": stores the results in the pointers passed to it,
 * also receives "data" which is the data field of the symbols node in the 
 * symbol table casted to "instruction" pointer, in order to determine the
 * number of parameters it receives. if any errors are detected, 0 is returned
 * and the error reporting function is called.
 */
static int detect_operands_and_types(instruction *data, int *op1_type, int *op2_type, char *op1, char *op2){
    int c, status = 1, comma_detected = 1;
    *op1_type = 0 ; *op2_type = 0;
    if (data->input) *op1_type = detect_operand_type(op1);
    if (data->input && data->output){
        skip_whites();        
        if (peek_next_char() == ',' ){
            c = getc(intput_as);
            skip_whites();
        }
        else
            status = comma_detected = 0;
    }
    if (data->output) *op2_type = detect_operand_type(op2); 
    if (status && ((c = peek_next_char()) == '\n' || c == EOF) && *op1_type != -1 && *op2_type != -1) skip_line();
    else status = 0;
    if (!status) detect_operands_error_check(op1_type, op2_type, c, comma_detected);
    return status;
}

/*
 * check_operands_types:
 * checks if the operands types ("op1_type" and "op2_type") passed to the instruction
 * suit the types it supports for input and output operands, this information is
 * encoded in "data" an "instruction" type defined in the symbols table file header.
 * in case this function detects an error it should return the linebreak character 
 * to the input, since it must have been called after "detect_operands_and_types"
 * which have found no error and skipped a line, this will make sure that the next line
 * wont be skipped as well.
 */
static int check_operands_types(instruction *data, int op1_type, int op2_type){
    int status = 1;
    if (data->input == 1 && (op1_type == IMMEDIATE || op1_type == REGISTER)){
        status = 0;
        print_error(line_count, 8);
    }

    if (data->output == 1 && op2_type == IMMEDIATE){
        status = 0;
        print_error(line_count, 9);        
    }
    if (!status)
        ungetc('\n', intput_as);        
    return status;
}

/*
 * create_instruction_word:
 * creates the word that should be stored in the instructions array of the data
 * memory, depending on the operands addressing types, the instruction's code
 * (represented by 4 bits) and the result is stored in "target" word.
 */
static void create_instruction_word(instruction *data, word *target, int op1_type, int op2_type){
    if (data->input)
        target->value +=  op1_type<< 4;
    if (data->output)
        target->value +=  op2_type<< 2;	
}

/*
 * extract_regs_value:
 * "symbol" is the name of the register ("rX"), and "is_input" indicates
 * if the register is passed as input or output operand. the function extracts
 * the word which represents the registers encoding as an operand from the 
 * data field of the registers symbol in the symbols table cast to "regs"
 * pointer type.
 */
static int extract_regs_value(char *symbol, int is_input){
    if (is_input)
        return ((regs*)(find_symbol(symbol)->data))->input_op.value;
    else
        return ((regs*)(find_symbol(symbol)->data))->output_op.value;
}

/*
 * is_number:
 * determines if the string "number" is indeed a natural signed number:
 * it can only have the characters '+' or '-' if they appear on the left 
 * followed by digits only, returns 1 if number is detected, 0 otherwise.
 */
static int is_number(char *number){
    int i, status = 1, first_char = number[0], length = strlen(number);
    if (length){        
        if(length == 1 && !isdigit(first_char)) status = 0;
        else if (!isdigit(first_char) && first_char != '+' && first_char != '-')
            status = 0;
        for(i = 1; i < length; i++){
            if (!(isdigit(number[i]))){
                status = 0;
                break;
            }
        }        
    }
    else status = 0;
    return status;
}

/*
 * process_struct:
 * stores the "op" as a struct in the instructions array of the memory manager.
 * at this stage (first pass) the address of the struct in the data array is 
 * unknown, so 0 is inserted, the next word should contain the address of the 
 * field: 1 for the number filed and 2 for the string, so the last character of
 * the string is checked and the proper value is set for the word to be stored
 * in the array: 00-000001-00 for 1 and 00-000010-00 for 2. if the label (the part
 * up to the '.') is legal, status is 1, 0 otherwise.
 */
static int process_struct(char *op){
    int status = 1, length = strlen(op);
    int index_part = op[length - 1] - '0';
    op[length - 2] = '\0';
    if ((status = is_legal_label(op, 0))){
        word temp_word = {0};
        spl_insert(op, get_ic(), line_count, 1);
        instructions_array_insert(temp_word);
        temp_word.value = index_part<<2;
        instructions_array_insert(temp_word);        
    }
    return status;
}

/*
 * process_immediate:
 * inserts the number operand in the instructions array after placing its value
 * in an 8 bit word (to make sure the operand will fit in the leftmost 8 bits
 * of the 10 bits word), if the value is out of range, a warning is printed, but 
 * still is not an error. if the "op" is indeed a number then status is 1, otherwise
 * 0 is returned by the function and an error is printed.
 */
static int process_immediate(char *op, word *temp_word){
    int status;
    union {int value :8;} item;
    int value = atoi(++op);
    item.value = value;
    temp_word->value = (item.value)<<2;
    instructions_array_insert(*temp_word);
    if (value > 127 || value < -128)
        print_warning_int(line_count, 1, value);
    status = is_number(op);
    if(!status) print_error_string(line_count, 10, op);
    return status;
}

/*
 * process_operand:
 * this function decides which action to take in order to store the operand "op"
 * in the instructions array based on its addressing "type": if it's either a number
 * ("IMMEDIATE") or a "STRUCT", the proper function is called, if it's "REGISTER" 
 * then it's encoding is stored (depending on "is_input" flag's value). if it's
 * of "ABSOLUTE" type: then 0 is inserted in the instructions array (the address
 * will be determined by second pass), the symbol and the current instructions
 * array index (where the word will be saved) is also stored in the second pass list
 * and the externs list also gets updated in order to produce the ".ext" file, in
 * case this operand turns out to be declared by ".extern" directive, status is
 * set to 1 if the "op" is a legal operand name, error reporting is done by the 
 * "is_legal_label" function.
 */
static int process_operand(int type, char *op, int is_input){
    int status = 1;
    word temp_word = {0};
    if (type == REGISTER){
        temp_word.value = extract_regs_value(op, is_input);
        instructions_array_insert(temp_word);
    }
    else if (type == IMMEDIATE) status = process_immediate(op, &temp_word);
    else if (type == ABSOLUTE && (status = is_legal_label(op, 0))) {
        spl_insert(op, get_ic(), line_count, 0);
        ent_ext_list_insert(op, 0, get_ic());
        instructions_array_insert(temp_word);
    }
    else if (type == STRUCT) status = process_struct(op);
    return status;
}

/*
 * store_operands:
 * responsible for storing the operands ("op1" and "op2") based on types ("op1_type"
 * and "op2_type") by calling "process_operand" for each of up to two operands (input
 * and output). the only special case that this function takes care of itself is
 * when both operands are registers and the words should be combined to one word,
 * in this case, their values are extracted, summed and stored. the integer value
 * returned indicates success (if the called function return success as well), otherwise
 * 0 is returned.
 */
static int store_operands(instruction *data, int op1_type, int op2_type, char *op1, char *op2){
    int status = 1;
    if (op1_type == REGISTER && op2_type == REGISTER){
        word temp_word = {0};
        temp_word.value += extract_regs_value(op1, 1) + extract_regs_value(op2, 0);
        instructions_array_insert(temp_word);
    }
    else {
        if (data->input) status = status && process_operand(op1_type, op1, 1);
        if (status && data->output) status = status && process_operand(op2_type, op2, 0);
    }
    return status;
}

/* 
 * process_instruction:
 * responsible for processing an instruction line by calling the relevant
 * functions: first, if a "label" is present (determined by "is_label" flag),
 * the label is inserted in the symbols table as an instruction label "INST_L".
 * then, "detect_operands_and_types" is called to detect the operands and their
 * types and to perform error checking, if no errors are detected, the instruction
 * word with the operands encoding is created and stored in the instructions array,
 * and "store_operands" is called to store the operands values and addresses, depending
 * on their encoding, otherwise, the line is skipped and 0 is returned to the calling
 * function.
 */
static int process_instruction(node *inst, int is_label, char *label){
    int op1_type = -1, op2_type = -1, status = 1;
    char op1[MAX_BUFFER_SIZE] = "", op2[MAX_BUFFER_SIZE] = "";
    instruction *data = (instruction*)(inst->data);
    word output_value = data->value;
    if (is_label) symbol_table_insert_label(label, get_ic(), INST_L, 0);
    if ((status = (detect_operands_and_types(data, &op1_type, &op2_type, op1, op2) && check_operands_types(data, op1_type, op2_type))))
        create_instruction_word(data,&output_value, op1_type, op2_type);
    if (!status) skip_line();
    else {
        instructions_array_insert(output_value);
        if (data->input || data->output)
            status = status && store_operands(data, op1_type, op2_type, op1, op2);
    }
    return status;
}

/*
 * process_line:
 * checks the lines type and determines which function to call: if the line 
 * is blank or is a comment, it is skipped, otherwise, "pre_process_line" is
 * called to determine the command type and if a label is present, and if no
 * errors are detected, the proper command processing function is called, either
 * a directive or an instruction. each function called down the line should
 * return its status of success: if no errors are detected by one of them,
 * the chain of functions called are responsible for skipping the line, if an
 * error is detected, the line processing should stop and this function must do
 * the line skipping part.
 */
static int process_line(void){
    char label[MAX_BUFFER_SIZE], command[MAX_BUFFER_SIZE];
    int label_flag = 0, status =1;
    node *symbol;
    if (is_comment_or_empty_line() || peek_next_char() == EOF){
        skip_line();
        return status;        
    }
    if ((status = pre_process_line(label, command, &label_flag, &symbol))){
        if (symbol->type == INST)
            status = process_instruction(symbol, label_flag, label);
        else if (symbol->type == DIRECT)
            status = process_directive(symbol, label_flag, label);
    }
    else
       skip_line();
    return status;
}

/*
 * first_pass_process:
 * this function calls "process_line" on each new line in the input ".as" file.
 * if any line reports a status of 0, then this function returns 0 so no output
 * files will be produced, since an error was detected and reported. the function
 * does not stop if a line had an error, so each line which has at least one error
 * is reported. "line_count" is incremented with each new line detected and the
 * function stops when EOF is detected in one of the lines.
 */
int first_pass_process(void){
    int status;
    status = 1;
    if (intput_as){
        int temp_status;
        while(!feof(intput_as)){
            line_count++;
            if (!(temp_status = process_line()))
                status = 0;
        }
    }
    printf("\nLines Processed : %d\nFirst pass status: %s\n", line_count, status ? "Success" : "Failure");
    return status;
}