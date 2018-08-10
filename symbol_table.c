#include "symbol_table.h"

/*
 * This module implements a symbol table for the assembler's use: it is 
 * actually a hash table that includes all the symbols that occur during
 * the assembly process, it contains all the commands, registers, labels,
 * etc.. the nodes might contain different "data" fields depending on their
 * role, some are defined in the header of this file. since the program needs
 * one such table, it's constructed in this module and is accessible only to
 * it. some functions are also static, since they are not needed outside this
 * module. the table is initialized when the program starts working on a new file,
 * and should be destroyed when done, the initializer and destructor are called
 * by the user.
 */
 
 /* this is the hash table itself and will only be visible to this file.
  * it is explicitly initialized to NULL (though static variables are  
  * implicitly initialized to 0 in C).
  */
static hash_table *symbol_table = NULL;

/*
 * construct_instruction:
 * creates a new "instruction" which contains the information relevant to an
 * "instruction" command: its "input" and "output" supported addressing types,
 * and its "word_value" equivalent: 4 bits (there are 16 in total) shifted 6 bits
 * to the left, to allow the addition of the operands addressing type encoding
 * to the instruction word (e.g. "add" will have the value 0010-0000-00). this
 * function returns a pointer to the "instruction", which will be stored in the
 * nodes data feild.
 */
static instruction *construct_instruction(int input, int output, int word_value){
    word new_word;
    instruction *data;
    word_value <<= 6;
    new_word.value = word_value;
    data = (instruction*)malloc(sizeof(instruction));
    if(data){
        data->input = input;
        data->output = output;
        data->value = new_word;
        return data;
    }
    else return exit_program_fatal_error();
}

/*
 * load_instructions:
 * loads the 16 instructions into the symbol_table, and calls "construct_instruction"
 * for each, to create the "instruction" with the proper information. the instruction
 * is casted to void pointer before it's passed to "hash_table_insert". "INST" is
 * defined in the "type" enumeration in the header, and represents the node's symbol type.
 */
static void load_instructions(void){
    hash_table_insert(symbol_table, "mov", (void *)construct_instruction(2, 1, 0), INST);
    hash_table_insert(symbol_table, "cmp", (void *)construct_instruction(2, 2, 1), INST);
    hash_table_insert(symbol_table, "add", (void *)construct_instruction(2, 1, 2), INST);
    hash_table_insert(symbol_table, "sub", (void *)construct_instruction(2, 1, 3), INST);
    hash_table_insert(symbol_table, "not", (void *)construct_instruction(0, 1, 4), INST);
    hash_table_insert(symbol_table, "clr", (void *)construct_instruction(0, 1, 5), INST);
    hash_table_insert(symbol_table, "lea", (void *)construct_instruction(1, 1, 6), INST);
    hash_table_insert(symbol_table, "inc", (void *)construct_instruction(0, 1, 7), INST);
    hash_table_insert(symbol_table, "dec", (void *)construct_instruction(0, 1, 8), INST);
    hash_table_insert(symbol_table, "jmp", (void *)construct_instruction(0, 1, 9), INST);
    hash_table_insert(symbol_table, "bne", (void *)construct_instruction(0, 1, 10), INST);
    hash_table_insert(symbol_table, "red", (void *)construct_instruction(0, 1, 11), INST);
    hash_table_insert(symbol_table, "prn", (void *)construct_instruction(0, 2, 12), INST);
    hash_table_insert(symbol_table, "jsr", (void *)construct_instruction(0, 1, 13), INST);
    hash_table_insert(symbol_table, "rts", (void *)construct_instruction(0, 0, 14), INST);
    hash_table_insert(symbol_table, "stop", (void *)construct_instruction(0, 0, 15), INST);
}

/*
 * load_psw_register:
 * creates a register named "PSW" and loads it into the symbol table, this is 
 * a private case of the function "load_registers". the register code is the highest
 * possible and is equal to "REGISTERS_COUNT" which is set to 8 by default: so if
 * registers r0-r7 will occupy codes/addresses 0-7, PSW will occupy code 8.
 */
static void load_psw_register(void){
    int temp;
    regs *data;
    word input, output;
    char code[4] = "PSW";
    data = (regs*)malloc(sizeof(regs));
    temp = REGISTERS_COUNT;
    input.value = temp<<6;
    output.value = temp<<2;
    if (data){
        data->input_op = input;
        data->output_op = output;
        hash_table_insert(symbol_table, code, (void *)data, REGS);
    }
    else exit_program_fatal_error();    
}

/*
 * load_registers:
 * creates "count" "regs" (which represent a register, defined in the file header),
 * and inserts them into the symbol table. each "regs" contains the encoding for
 * when it's an input and an output operand, for example: r1 will have the value
 * 0000-01-00-00 when it's passed as input operand and 0000-00-01-00 for output.
 * "REGS" is defined in the enumeration "type".
 * the function also calls "load_psw_register".
 */
static void load_registers(int count){
    int i;
    for (i = 0; i < count; i++){
        regs *data;
        char code[3] = "rX";
        int temp1 = i<<6, temp2 = i<<2;
        word input, output;
        code[1] = '0' + i;
        data = (regs*)malloc(sizeof(regs));
        input.value = temp1;
        output.value = temp2;
        if (data){
            data->input_op = input;
            data->output_op = output;
            hash_table_insert(symbol_table, code, (void *)data, REGS);
        }
        else exit_program_fatal_error();
    }
    load_psw_register();
}

/*
 * construct_directive:
 * creates a "directive" and returns a pointer to it, a directive is a keyword used
 * to store data or create a new label for the assembler, "directive" data type
 * contains information about the directive, and is stored into the node's data
 * field.
 */
static directive *construct_directive(int integers, int string, int label){
    directive *data = (directive*)malloc(sizeof(directive));
    if (data){
        data->integers = integers;
        data->string = string;
        data->label = label;
        return data;       
    }
    else return exit_program_fatal_error();
}

/*
 * load_directives:
 * loads the 5 directives keywords into the symbol table using "construct_directive",
 * whose result is cast to void pointer, as with "load_instructions". "DIRECT" is
 * defined in the enumeration "type".
 */
static void load_directives(void){
    hash_table_insert(symbol_table, ".data", (void *)construct_directive(1, 0, 0), DIRECT);
    hash_table_insert(symbol_table, ".struct", (void *)construct_directive(1, 1, 0), DIRECT);
    hash_table_insert(symbol_table, ".string", (void *)construct_directive(0, 1, 0), DIRECT);
    hash_table_insert(symbol_table, ".entry", (void *)construct_directive(0, 0, 1), DIRECT);
    hash_table_insert(symbol_table, ".extern", (void *)construct_directive(0, 0, 1), DIRECT);
}

/*
 * initialize_symbol_table:
 * initializes the symbol_table by calling the hash table constructor and storing
 * its return value in "symbol_table". the function then loads the instructions,
 * directives and registers by calling the relevant static functions. this function
 * should be called each time a new file needs to be processed by the assembler.
 * by default the are 8 registers "REGISTERS_COUNT".
 */
void initialize_symbol_table(void){
    symbol_table = hash_table_construct(DEFAULT_SIZE, default_hash_function);
    load_instructions();
    load_registers(REGISTERS_COUNT);
    load_directives();
}

/*
 * symbol_table_insert_label:
 * creates and inserts a new symbol into the table, the nodes "data" field is
 * a "label" (defined in the file's header), its contents depends on the "type" 
 * passed. the types are defined in the "type" enumeration in the file's header:  
 * "EXTERN" represents a symbol defined by the ".extern" directive, "INST_L" is a 
 * label that appears before an instruction, and "DATA" represents a symbol that 
 * marks a label defined by one of the directives: ".data", ".struct" or ".string".
 * the word value set for an "EXTERN" type is 00000000-01, which resembles its
 * address for later use. for "INST_L" the word value has no meaning, but still the
 * "Instructions Counter" is passed for any potential future use. for "DATA" types,
 * the word value is the "Data Counter", which will be extracted later by the file
 * second pass processor. "is_struct" is a flag that marks a ".struct" and will be
 * also used by the second pass processor.
 */
void symbol_table_insert_label(char *symbol, int counter, int type, int is_struct){
    label *data = (label*)malloc(sizeof(label));
    if (data){
        word new_word;
        if (type == EXTERN)
            new_word.value = 1;
        else if (type == INST_L)
            new_word.value = counter;
        else
            new_word.value = counter;
        if (data) {
            data->address = new_word;
            data->is_struct = is_struct;
        }
        hash_table_insert(symbol_table, symbol, (void *)data, type);        
    }
    else exit_program_fatal_error();
}

/*
 * free_symbol_table:
 * frees the symbol_table, by calling the hash_table destructor and sets
 * its value to NULL, in case a new file will be processed "initialize_symbol_table"
 * should be called again. this should be called by the file processing is done.
 */
void free_symbol_table(void){
    hash_table_free(symbol_table);
    symbol_table = NULL;
}

/*
 * find_symbol:
 * a wrapper for "hash_table_find", to look for "symbol" in the "symbol_table".
 */
node *find_symbol(char *symbol){
    return hash_table_find(symbol_table, symbol);
}