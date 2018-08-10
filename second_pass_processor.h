#ifndef SECOND_PASS_PROCESSOR_H
#define SECOND_PASS_PROCESSOR_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "symbol_table.h"
    #include "memory_manager.h"
    #include "error_handler.h"
	
    /* a struct which contains information to be stored in the "data"
    * field of a "node" that belongs to the "second_pass_list". */	 
    typedef struct line_data {
        int line_count : 10;
        unsigned int is_struct : 1;
    } line_data;    
    
    void initialize_second_pass_lists(void);
    void free_second_pass_lists(void);
    node *ent_ext_list_insert(char*, int, int);
    node *entries_list_find(char*);
    int second_pass_process(void);
    void create_entries_file(char*);
    void create_externs_files(char*);
    node *spl_insert(char*, int, int, int);
            
#endif