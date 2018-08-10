#include "hash_table.h"

/*
 * this module implements a simple hash table that uses a single
 * array that contains pointers to linked lists: the hash function
 * is assumed to be universal, it should return an integer which 
 * corresponds to one if the linked lists (buckets) of the array.
 * collisions in this hash table are resolved by chaining, so if
 * two keys have the same hash value, they both will be inserted
 * in the same bucket. if the hash function is indeed universal,
 * the mean running time of each of the dictionary operations (searh,
 * insert, delete) is of constant complexity (or linear to the hash
 * function runtime if the length of the key is unlimited).
 */
 
/*
 * default_hash_function:
 * takes a string "key" and the size of the hash table's array "array_size",
 * and returns an integer, which is the hash value for the given
 * key: simply the remainder from the devision of the sum of the key's 
 * characters values by the array's size (range is 0 - "array_size"-1), so
 * the value output value would represent a valid array index. nothing fancy.
 */
int default_hash_function(char *key, int array_size){
    int c, hash_value = 0;
    while((c = *key++))
        hash_value += c;
    return hash_value%array_size;
}

/*
 * hash_table_construct:
 * constructs a new hash table with an array of size "size" and a hash function
 * "function" (default_hash_function will be used by default). the hash table
 * is an array (pointer) of pointers to linked lists, the function allocates
 * memory for the array itself and the linked lists as well.
 */
hash_table *hash_table_construct(int size, int (*function)(char*, int)){
    int i;
    hash_table *table = (hash_table*)malloc(sizeof(hash_table));
    linked_list** array= (linked_list**)malloc(size*sizeof(linked_list*));
    if (table && array){
        table->array_size = size;
        table->function = function;
        table->array = array;
        for(i = 0; i < size; i++)
            (table->array)[i] = linked_list_construct();    
        return table;        
    }
    else return exit_program_fatal_error();
}

/*
 * hash_table_free:
 * frees the memory allocated to "table" and all of its members and their
 * members.
 */
void hash_table_free(hash_table *table){
    int i;
    for(i = 0; i < table->array_size; i++)
        free_linked_list((table->array)[i]);
    free(table->array);
    free(table);
}

/*1 for success, 0 otherwise, remove printf statement*/
/*
 * hash_table_insert:
 * this table does not implement delete and does not allow duplicate keys,
 * so if a key is already present, a new node will not be created to replace
 * the existing one, instead an error is printed to stderr and 0 is returned,
 * otherwise, a new node is constructed with "key", "type" and "data" fields
 * passed by the user, and is inserted into the linked list which corresponds
 * to the hash value of the key, and 1 is returned. the caller is responsible
 * for handling the failure to insert a duplicate value.
 */
int hash_table_insert(hash_table *table, char *key, void *data, int type){
    linked_list *list;
    node *item;
    list = (table->array)[(table->function)(key, table->array_size)];
    if (!linked_list_find(list, key)){
        item = node_construct(key, type);
        item->data = data;
        linked_list_insert(list, item);
        return 1;
    }
    else{
        fprintf(stderr, "Error: item is already present: %s\n", key);
        return 0;   
    }
}

/*
 * hash_table_find:
 * searches "table" for a node with "key" as its key. the functions finds
 * the list at the index corresponding to the hash value of key: returns
 * a pointer to a node if such item exists or NULL other wise (using "linked_list_find").
 */
node *hash_table_find(hash_table *table, char *key){
    linked_list *list = (table->array)[(table->function)(key, table->array_size)];
    return linked_list_find(list, key);
}