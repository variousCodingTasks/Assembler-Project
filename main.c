#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "symbol_table.h"
#include "memory_manager.h"
#include "first_pass_processor.h"
#include "second_pass_processor.h"
#include "error_handler.h"

char *add_extension(char*, char*);
void file_process(char*);
void process_files(int, char**);
void free_string_aray(char**, int);

int main(int argc, char** argv) {    
    
    /*test*/
    
    /*file_process("test");*/
    /*file_process("test - copy");*/
    /*process_files(argc, argv);*/
    
    char *args[] = {" ", ".\\Examples\\Error_Input_1"};
    process_files(2, args);
    
    return (EXIT_SUCCESS);
}

/*
 * add_extension:
 * creates a new string named "output" large enough to hold both "string" and
 * "extension" when combined to one string, and combines them to one string,
 * returning the result string.
 */
char *add_extension(char *string, char *extension){
    char *output;
    output = (char*)malloc((1 + strlen(extension)+strlen(string))*sizeof(char));
    strcpy(output, string);
    strcat(output, extension);
    return output;
}

/*
 * file_process:
 * takes "filename" string which does not include the ".as"  extension at its end 
 * adds the extensions, using "add_extension" function.
 * it then loads the different components required for input processing: symbols table,
 * memory, file pointers and second pass lists, then calls the first and second
 * pass processors. if no errors were detected by either of the processors the output
 * files are produced. the ".ob" and ".ext" are guaranteed to be error free if the
 * program decides to produce them, the ".ent" file creator might still report an error
 * and will not be produced if a certain ".entry" directive's label (operand) was
 * not defined in the input file. if there was a problem opening the input file,
 * an error is printed to stderr.
 */
void file_process(char *filename){
    int status = 1;
    char *files_names[4];
    if (load_input_file(files_names[0] = add_extension(filename, ".as"))){
        initialize_symbol_table();
        initialize_memory();    
        initialize_second_pass_lists();
        status = first_pass_process() && second_pass_process() && !get_memmory_full_flag();
        if (status){
            save_memory_to_file(files_names[1] = add_extension(filename, ".ob"));
            create_entries_file(files_names[2] = add_extension(filename, ".ent"));
            create_externs_files(files_names[3] = add_extension(filename, ".ext"));
            free_string_aray(files_names, 4);
        }        
        free_symbol_table();
        free_memory();
        close_input_file();
        free_second_pass_lists();
    }
    else fprintf(stderr, "Error: unable to open file \"%s\".\n", filename);
}

/*
 * process_files:
 * this function goes through the command line operands and calls "file_process"
 * on each, starting from the first to the last.
 */
void process_files(int argc, char** argv){
    int i = argc;
    while (0 < --i){
        fprintf(stdout, "\nProcessing file \"%s.as\"...\n\n", argv[argc - i]);
        file_process(argv[argc - i]);
        fprintf(stdout, "\nDone processing file \"%s.as\".\n\n", argv[argc - i]);
    }
}

/*
 * free_string_aray:
 * frees each of the character pointers (strings) in "array" with
 * a given size, assumes that the pointers were assigned dynamically.
 */
void free_string_aray(char **array, int size){
    int i;
    for(i = 0; i < size; i++)
        free(array[i]);
}
    
