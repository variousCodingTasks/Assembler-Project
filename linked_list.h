#ifndef LINKED_LIST_H
#define LINKED_LIST_H

    #include <stdio.h> 
    #include <stdlib.h>
    #include <string.h>
    #include "error_handler.h"

    /*set according to the maximum label length allowed*/
    #define MAX_NAME_SIZE 31

    /*
     * node type:
     * this struct contains the fields of a linked list node.
     * the "type" and "index" fields will be used for different
     * purposes, depending on the context. "next" points to the next
     * node in the list, the last node should point to NULL. "key"
     * is a string that represents the item, MAX_NAME_SIZE is set
     * so the key would be able to hold the maximum label length
     * allowed in addition to the terminating character. the "data"
     * field might be used to hold additional information, depending
     * on context.
     */	
    typedef struct node {
        char key[MAX_NAME_SIZE];
        int type;
        int index : 10;
        void *data;
        struct node *next;
    } node;
	
    /*
     * lined_list type:
     * represents a linked list: stores a pointer to the first node
     * and NULL if empty.
     */
    typedef struct linked_list {
        node *head;
    } linked_list;
    
    linked_list *linked_list_construct(void);
    node *node_construct(char*, int);
    void free_linked_list(linked_list*);
    void linked_list_insert(linked_list*, node*);
    node *linked_list_find(linked_list*, char*);
    void traverse_list(linked_list*);
    void reverse_list(linked_list*);

#endif