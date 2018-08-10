#ifndef SYMBOL_TABLE_H
#define SYMBOL_TABLE_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "word.h"
    #include "hash_table.h"    

    #define REGISTERS_COUNT 8

    typedef enum type {INST, REGS, EXTERN, DATA, INST_L, DIRECT} type;
	
	/*
	 * there are 4 types of addressing allowed (immediate, absolute, struct and
	 * register). for the "input" field: 0 for no input, 1 for types 1-2, and
	 * 3 for types 0-3. for "output": 0 for no output, 1 for 1-3 and 2 for 0-3.
	 * the word "value" depends on the context. this should be placed in a node's
	 * "data" filed. this data type goes with "INST" type.
	 */	
    typedef struct instruction {
        unsigned int input : 2;
        unsigned int output : 2;
        word value;
    } instruction;
 
 	/*
	 * represents a register "REGS" and should be placed in its node "data" 
	 * field in the symbol table.
	 */	
    typedef struct regs {
        word input_op;
        word output_op;
    } regs;
    
 	/*
	 * represents a "DATA", "EXTERN" or "INST_L" label and should be placed 
	 * in its node "data" field in the symbol table.
	 */		
    typedef struct label {
        word address;
        unsigned int is_struct : 1;
    } label;
    
 	/*
	 * represents a "DIRECT", and should be placed in its node "data" field in 
	 * the symbol table. the fields were not used but were still kept for any 
	 * possible future use.
	 */			
    typedef struct directive {
        int integers;
        unsigned int string : 1;
        unsigned int label : 1;
    } directive;
    
    void initialize_symbol_table(void);
    void symbol_table_insert_label(char*, int, int, int);
    node *find_symbol(char*);
    void free_symbol_table(void);

#endif