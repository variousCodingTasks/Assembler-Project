#include "second_pass_processor.h"

/*
 * the objects and data structures in the file should be created and
 * destroyed for each file that needs processing. this module has 3
 * main roles:
 * 1. substituting all the data and external variable values in the
 * instruction array (in memory manager) with the correct addresses.
 * up to this point, their values were set to 0 by the first pass processor.
 * 2. creating the ".ent" file.
 * 3. reading the ".ext" file.
 * this is the second processing phase and should be called after first pass
 * processing is done. each time the first pass processor encountered an operand 
 * of "ABSOLUTE" type in an instruction, it saved a reference to it while setting 
 * the word value at the instruction array to 0. at the beginning of the second
 * phase, the symbol table should contain all the relevant labels, if everything
 * went fine so far, so the second pass traverses the array that stores the references
 * and substitutes the 0 values with the correct addresses (in addition to 2 bits 
 * that indicate the A,R,E correct value for the operand) in the instructions array.
 * the first pass processor also saved a reference for each time it encountered the
 * ".entry" directive in a linked list, so it also traverses this list and creates
 * the ".ent" file, using the addresses stored in the symbols table.
 * the first pass processor also saved a reference each time it encountered an operand
 * of "ABSOLUTE" type in an instruction. this list is traversed and each of its nodes
 * is checked to contain a key which represents an EXTERN variable, if so, the symbol
 * and the address of the symbol in the instructions array is written to the ".ext"
 * file. a separate list is used instead of the "second_pass_list" for convenience
 * purposes only, so the list can be manipulated (e.g. reversed) without the risk of
 * corrupting of the "second_pass_list" (second_pass_list and externs_list contain
 * different kinds of information and are not identical) and accessing the relevant
 * address more easily.
 */
 
/*
 * "second_pass_list": is a linked list which contains references to all occurrences
 * of "ABSOLUTE" type operands in instructions and additional data these occurrences.
 * "entries_list": a linked list that contains information about each occurrence of
 * the ".entry" directive in the file.
 * "externs_list": a linked list that contains information about each occurrence of
 * an "ABSOLUTE" type operand for the ".ext" file creation process.
 */ 
static linked_list *second_pass_list = NULL;
static linked_list *entries_list = NULL;
static linked_list *externs_list = NULL;

/*
 * initialize_second_pass_lists:
 * constructs 3 empty linked lists and assigns them to the file's static
 * lined lists. should be called before beginning new file processing.
 */
void initialize_second_pass_lists(void){
    second_pass_list = linked_list_construct();
    entries_list = linked_list_construct();
    externs_list = linked_list_construct();
}


/*
 * free_second_pass_lists
 * frees the file's linked_list's allocated memory and sets their value to NULL
 * for the next file processing. should be called when current file processing
 * is finished.
 */
void free_second_pass_lists(void){
    free_linked_list(second_pass_list);
    free_linked_list(entries_list);
    free_linked_list(externs_list);
    second_pass_list = NULL;
    entries_list = NULL;
    externs_list = NULL;
}

/*
 * spl_insert:
 * inserts a new node into "second_pass_list". "line_data" type contains
 * information which will be used for error checking, mainly to check if
 * what appears as a ".struct" operand indeed refers to a ".struct" data type.
 * the function creates a new "node" with its key field set to "key" and
 * "type" to 0 (since it wont be used). it also creates a new "line_data"
 * to store the flag "is_struct" and "line_count" (for error reporting).
 * the "line_data" is assigned to the new node's "data" field after being
 * cast to void pointer. the new node's "index" field is set to "inst_index",
 * which refers to the operands word index in the instruction array (in the
 * memory manager). returns a node to the newly constructed node.
 */
node *spl_insert(char *key, int inst_index, int line_count, int is_struct){
    node *new_node = node_construct(key, 0);
    line_data *data = (line_data*)malloc(sizeof(line_data));
    if (data){
        new_node->index = inst_index;
        data->line_count = line_count;
        data->is_struct = is_struct;
        new_node->data = (void*)data;
        linked_list_insert(second_pass_list, new_node);
        return new_node;
    }
    else return exit_program_fatal_error();
}

/*
 * ent_ext_list_insert:
 * inserts a new node into the specified linked list ("is_ent": 1 for entries
 * list, 0 for externs list), where the key is "symbol" and "counter" as the
 * node's "index" field, which has different meaning, depending on the item
 * and list being added. a pointer to the newly constructed node is returned.
 */
node *ent_ext_list_insert(char *symbol, int is_ent, int counter){
    node *new_node = node_construct(symbol, 0);
    if (new_node) new_node->index = counter;
    if(is_ent)
        linked_list_insert(entries_list, new_node);
    else    {
        linked_list_insert(externs_list, new_node);
    }        
    return new_node;    
}

/*
 * entries_list_find:
 * looks for "symbol" in the entries list and returns a pointer to
 * the node if exists or NULL otherwise.
 */
node *entries_list_find(char *symbol){
    return linked_list_find(entries_list, symbol);
}

/*
 * extract_address:
 * looks for "symbol" in the externs list and returns a pointer to
 * the node if exists or NULL otherwise.
 */
static int extract_address(node *symbol){
    return ((label*)(symbol->data))->address.value;
}

/*
 * print_second_pass_error:
 * a wrapper for "print_second_pass_error" defined in the "error_handler" file.
 * it takes "node" instead, of an integer representing line count, and extracts
 * and extracts the "line_count" field from its "date field", after casting it
 * to "line_data" type. also sets the status pointer passed to 0 to indicate
 * to the calling function that there was an error.
 */
static void print_second_pass_error(int *status, node *curr, int error){
    *status = 0;
    print_error_string(((line_data*)(curr->data))->line_count, error, curr->key);
}

/*
 * second_pass_struct:
 * used to process an operand entered as a struct and stored in the 
 * "second_pass_list" as such. the function checks if the "symbol" found
 * indeed belongs to a struct (not any other kind of data, register, command..)
 * if not so, an error is printed, otherwise the address of the symbol is
 * extracted and stored in the right place in the instruction array.
 */
static void second_pass_struct(node *curr, node *symbol, int *status){
    if (symbol->type == DATA && ((label*)(symbol->data))->is_struct == 1){
        int address = C + get_ic() + extract_address(symbol);
        instructions_array_set_index(curr->index , ((address<<2) + 2));
    }
    else print_second_pass_error(status, curr, 28);
}

/*
 * second_pass_process:
 * this function traverses the "second_pass_list" and checks if the symbol
 * ("key)" stored in each node contains a valid label present in the symbols
 * table. if the symbol does not exist, an error is printed.
 * if a symbol exists: if the "curr" node refers to a .struct, "second_pass_struct"
 * is called to process it, otherwise, if the symbol type is "EXTERN",
 * the address (00-000000-01) is extracted and stored in the data array in the
 * original IC index when the operand was processed. if it's of type "DATA" (either
 * .data or .string) the index is extracted from the symbol (the one found in the
 * symbols table) in addition to starting index (C : 100) and the last IC (the total
 * is the final address of the variable in the data array) and this value is set
 * in the instructions array of the memory manager. the address is shifted two
 * bits to the left to add the A,R,E encoding which is supposed to be equal to
 * 10 (2 in decimal). if a symbol was found and it's not of the former two types,
 * then it must be either a command, directive or register so a proper error
 * is printed.
 * if an error is detected the function doesn't stop and keeps processing the list,
 * the integer returned indicates if any errors occurred to the caller.
 * this function will not be called in case first pass has failed.
 */
int second_pass_process(void){
    int status = 1;
    node *symbol, *curr;
    reverse_list(second_pass_list);
    curr = (second_pass_list != NULL) ? second_pass_list->head : NULL;
    while(curr){
        if ((symbol = find_symbol(curr->key))){
            if (((line_data*)(curr->data))->is_struct == 1) second_pass_struct(curr, symbol, &status);
            else if (symbol->type == EXTERN)
                instructions_array_set_index(curr->index , extract_address(symbol));
            else if (symbol->type == DATA){
                int address = C + get_ic() + extract_address(symbol);
                instructions_array_set_index(curr->index , ((address<<2) + 2));
            }
            else print_second_pass_error(&status, curr, 27);
        }
        else print_second_pass_error(&status, curr, 26);
        curr = curr->next;
    }
    printf("\nSecond pass status: %s\n", status ? "Success" : "Failure");
    return status;
}

/*
 * print_entries_file_error:
 * a wrapper for "print_second_pass_error" defined in the "error_handler" file.
 * sets the status pointer passed to 0 to indicate to the calling function that
 * there was an error. it extracts the "index" field of "curr" which is actually
 * the line_count.
 */
static void print_entries_file_error(int *status, node *curr, int error){
    *status = 0;
    print_error_string(curr->index , error, curr->key);
}

/*
 * create_entries_file:
 * "filename" is the name of the file this function creates to save the entries
 * data. the function creates a new file, reverses the entries list so it appears
 * in the original order (as items appear in the source file), the list is then 
 * traversed and each node is checked: if a symbol with same key does not exist
 * in the symbols table, an error is printed, if a symbol exists, this symbol is checked
 * if it contains a label of either "DATA" or "INST_L" (a label which appears before
 * an instruction), if so, the symbol is printed along with its address (IC is added
 * in case the label is in the data section), if the symbol represents any other
 * type, an error is printed. "lines_count" is incremented each time a line is actually
 * written to the file, if it's equal to 0 or any errors occurred (status is 0),
 * the file is removed. the processing stops only when the list is exhausted, and
 * keeps going on even if errors have been detected.
 * there might be situations were an ".ob" file is created while an error prevented
 * the assembler from creating ".ent" file: in this case the ".ob" file is not removed,
 * but is notified that errors have occurred trying to create the entries file, so the
 * user decides what to do.
 */
void create_entries_file(char *filename){
    int status = 1, lines_count =0;
    FILE *entries_file = fopen(filename, "w");
    node *symbol, *curr;
    reverse_list(entries_list);
    curr = entries_list->head;    
    if (entries_list){
        while(curr){
            symbol = find_symbol(curr->key);
            if (symbol) {
                char temp_str[MAX_NAME_SIZE];
                if (symbol->type == DATA) fprintf(entries_file, "%s %s\n", curr->key, convert_int_to_awkward_base(C + get_ic() + extract_address(symbol), temp_str));
                else if (symbol->type == INST_L) fprintf(entries_file, "%s %s\n", curr->key, convert_int_to_awkward_base(C + extract_address(symbol), temp_str));
                else print_entries_file_error(&status, curr, 30);
                if (status) lines_count++;
            }
            else print_entries_file_error(&status, curr, 29);
            curr = curr->next;
        }
        fclose(entries_file);
        if (!status || !lines_count) remove(filename);        
    }
    else fprintf(stderr, "Error: unable to create the file \"%s\".\n", filename);
}

/*
 * create_externs_files:
 * "filename" is the name of the file this function creates to save the entries
 * data. the function creates a new file, reverses the externs list so it appears
 * in the original order (as items appear in the source file), the list is then 
 * traversed and each node is checked: if a symbol with same key does not exist
 * in the symbols table, nothing happens, since such an error would have been
 * already detected by "second_pass_process", if a symbol exists, this symbol 
 * is checked if it contains a label of "EXTERN" type, which was declared using
 * ".extern" directive, the address of the operand in the instructions array
 * (plus L) is printed to the file and "lines_count" is incremented. it is worth
 * noting that first pass processing wont let and extern variable's name
 * collude with another variable or symbol name, so this algorithm is correct,
 * and ensures all occurrences of each extern variable in the instructions section
 * is properly recorded and stored in the ".ext" file, in case no other errors
 * have occurred.
 * if no lines were written to the file, it is removed.
 */
void create_externs_files(char *filename){
    int lines_count = 0;
    node *symbol, *curr;
    FILE *externs_file = fopen(filename, "w");
    reverse_list(externs_list);
    curr = externs_list->head;
    if (externs_file){
        while(curr){
            symbol = find_symbol(curr->key);
            if (symbol) {
                char temp_str[MAX_NAME_SIZE];
                if (symbol->type == EXTERN){
                    fprintf(externs_file, "%s %s\n", curr->key, convert_int_to_awkward_base(C + curr->index , temp_str));
                    lines_count++;
                }
            }        
            curr = curr->next;
        }
        fclose(externs_file);
        if (!lines_count) remove(filename);
    }
    else fprintf(stderr, "Error: unable to create the file \"%s\".\n", filename);
}