#ifndef MEMORY_MANAGER_H
#define MEMORY_MANAGER_H
    
    #include <stdio.h>
    #include <stdlib.h>
    #include "symbol_table.h"

    /*the starting index of the memory list*/
    #define C 100
    /*the maximum total items allowed in both instructions and data arrays*/
    #define MEMORY_SIZE 256
    
    void initialize_memory(void);
    void free_memory(void);
    int get_ic(void);
    int get_dc(void);
    void instructions_array_insert(word);
    void data_array_insert(word);
    void save_memory_to_file(char*);
    int get_memmory_full_flag(void);
    void instructions_array_set_index(int, int);

#endif