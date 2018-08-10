#ifndef FILE_HANDLER_H
#define FILE_HANDLER_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include <ctype.h>
    #include "symbol_table.h"
    #include "memory_manager.h"
    #include "second_pass_processor.h"
    
    /*the max string size allowed for a string, as the maximum line width allowed*/
    #define MAX_BUFFER_SIZE 80

    typedef enum addressing_type {IMMEDIATE, ABSOLUTE, STRUCT, REGISTER } addressing_type;

    FILE *load_input_file(char*);
    void close_input_file(void);
    int first_pass_process(void);
   
#endif