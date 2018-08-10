#ifndef HASH_TABLE_H
#define HASH_TABLE_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
    #include "linked_list.h"

    /*
     * the default size of the hash table array: a prime number that
     * is not close to a whole power of 2 and will result in a tolerable
     * mean number of probes per search operation.
     */	
    #define DEFAULT_SIZE 41
    /*
    * a hash_table type contains a pointer to an "array" of linked_lists
    * pointers, a hash "function" and the size of the array.
    */		
    typedef struct hash_table {
        linked_list **array;
        int (*function)(char*, int);
        int array_size;
    } hash_table;

    int default_hash_function(char*, int);
    hash_table *hash_table_construct(int, int (*)(char*, int));
    void hash_table_free(hash_table*);
    int hash_table_insert(hash_table*, char*, void*, int);
    node *hash_table_find(hash_table*, char*);

#endif