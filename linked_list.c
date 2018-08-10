#include "linked_list.h"

/*
 * this module implements a simple linked list: its header contains two
 * structures definitions, one for the list, to mark its "head" and one 
 * for the nodes which contain the actual data. this is a singly linked
 * list and each node contains a pointer to the next one.
 */

/*
 * linked_list_construct:
 * constructs a new linked list and returns a new pointer to it, the list
 * is initially empty, so its head is set to NULL. 
 */ 
linked_list *linked_list_construct(void){
    linked_list *list = (linked_list*)malloc(sizeof(linked_list));
    if (list){
        list->head = NULL;
        return list;
    }
    else return exit_program_fatal_error();
}

/*
 * node_construct:
 * creates a new node and sets the fields "type" and "key" as passed
 * by the user. it also sets the pointer to the next node to NULL, to
 * prevent issues in traversal and reversal of the list, the data field
 * is also set to NULL, in order to evade issues which might arise when
 * trying to free an uninitialized pointer, the new node is returned.
 */
node *node_construct(char *key, int type){
    node *item = (node*)malloc(sizeof(node));
    if (item) {
        strcpy(item->key, key);
        item->type = type;
        item->next = NULL;
        item->data = NULL;
        return item;
    }
    else return exit_program_fatal_error();
}

/*
 * free_linked_list:
 * this function receives a pointer to a list and frees all the dynamically
 * allocated members of it: the nodes, their data fields and finally the
 * list itself.
 */
void free_linked_list(linked_list *list){
    node *curr = list->head;
    while(curr) {
        node *temp = curr;
        curr = curr->next;
        free(temp->data);
        free(temp);
    }
    free(list);
}

/*
 * linked_list_insert:
 * inserts the new node "item" into the list: the item is pushed to
 * the top of the list, so it also becomes the new "head".
 */
void linked_list_insert(linked_list *list, node *item) {
    if (list->head)
        item->next = list->head;
    list->head = item;
}

/*
 * reverse_list:
 * this function receives a list pointer and reverses the order of its
 * nodes (the first becomes the last), it's useful when the user wants the
 * list's items sorted by insertion order.
 */
void reverse_list(linked_list *list){
    node *curr = list->head;
    node *prev = NULL;
    node *next;
    while (curr){
        next = curr->next;
        curr->next = prev;
        prev = curr;
        curr = next;
    }
    list->head = prev;
}

/*
 * linked_list_find:
 * searches "list" for a node with its key field set to "key", returns
 * a pointer to the node, in case found, or NULL otherwise.
 */
node *linked_list_find(linked_list *list, char *key){
    node *curr = list->head;
    while(curr) {
        if (!strcmp(key, curr->key))
            return curr;
        else
            curr = curr->next;
    }
    return NULL;
}

/*
 * traverse_list:
 * receives a pointer to a list and prints the keys and the types of its
 * each of its nodes to stdout. this is used for debugging purposes.
 */
void traverse_list(linked_list *list){
    node *curr = list->head;
    while(curr) {
        printf("%s (%d)\t", curr->key, curr->type);
        curr = curr->next;
    }
    puts("");
}