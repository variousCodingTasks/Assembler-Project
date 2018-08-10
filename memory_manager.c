#include "memory_manager.h"

/*
 * This module defines an object which resembles the imaginary computer's
 * memory. there's only one instant running, so there's no need to create
 * copies of the object, therefore static variables are used to store the 
 * state of the memory. this object is created and destroyed for each new
 * file that requires processing, it includes two data arrays that stores
 * "words", one for instructions and one for the data, the user is responsible
 * for making sure the data stored is not corrupt. this module is responsible
 * for creating and storing the ".ob" file since it includes all the data
 * required.
 */

/*
 * "instructions_array": this array stores the words which belong to instructions.
 * "data_array": this array stores the words that belong to directives.
 * "IC" and "DC": are counters that store the next available index in each
 * of the arrays.
 * "memory_full_flag" indicates whether the assembler tried to add a new word
 * to one of the arrays when the maximum total count has already been reached,
 * in other words, the memory was full.
 */
static word *instructions_array = NULL;
static word *data_array = NULL;
static int IC = 0;
static int DC = 0;
static int memory_full_flag = 0;

/*
 * initialize_memory:
 * creates two new arrays and assigns them to the arrays defined above.
 * should be called before beginning to process a new file.
 * "calloc" is used to make sure all members are initially set to 0.
 */
void initialize_memory(void){
    instructions_array = (word*)calloc(MEMORY_SIZE, sizeof(word));
    data_array = (word*)calloc(MEMORY_SIZE, sizeof(word));
    if (!(instructions_array && data_array))
        exit_program_fatal_error();
}

/*
 * free_memory:
 * should be called when file processing is done, frees the arrays and
 * makes sure all the file's variables are set to zero or NULL.
 */
void free_memory(void){
    free(instructions_array);
    free(data_array);
    instructions_array = NULL;
    data_array = NULL;
    IC = 0;
    DC = 0;
    memory_full_flag = 0;
}

/*
 * get_ic:
 * returns the current value of IC (which is also the count of the words
 * stored in the instructions array).
 */
int get_ic(void){
    return IC;
}

/*
 * get_dc:
 * returns the current value of DC (which is also the count of the words
 * stored in the data array).
 */
int get_dc(void){
    return DC;
}

/*
 * instructions_array_insert:
 * inserts "item" in the instructions arrays, if the total count is 
 * less than the maximum memory size allowed, an error is printed to
 * stderr, but the program will continue to run.
 * the total count is the total number of members in both arrays.
 */
void instructions_array_insert(word item){
    if (IC + DC <= MEMORY_SIZE)
        instructions_array[IC++] = item;
    else {
        memory_full_flag = 1;
        fprintf(stderr, "Error: memory is full.\n");        
    }
}

/*
 * data_array_insert:
 * inserts "item" in the instructions arrays, if the total count is 
 * less than the maximum memory size allowed, an error is printed to
 * stderr, but the program will continue to run.
 * the total count is the total number of members in both arrays.
 */
void data_array_insert(word item){
    if (IC + DC <= MEMORY_SIZE)
        data_array[DC++] = item;
    else {
        memory_full_flag = 1;
        fprintf(stderr, "Error: memory is full.\n");        
    }
}

/*
 * instructions_array_set_index:
 * sets the word's value at "index" to "address", this will be used
 * by the second pass processor to update the addresses of data and
 * external variables passed as operands to instructions.
 */
void instructions_array_set_index(int index, int address){
    word temp_word;
    temp_word.value = address;
    instructions_array[index] = temp_word;
}

/*
 * save_memory_to_file:
 * creates a file named "filename" and stores the contents of the two arrays,
 * encoded to the "awkward base". the first line will contain two words, the
 * first is the length of the instructions section and the second is the length
 * of the data section. the instructions array is traversed, each line contains
 * the index of that word (plus L, the starting index, set to 100 by default).
 * and the word itself.
 * the data array is then traversed and the index count is continued from where
 * it stopped in the instructions array, therefore, IC is added.
 * if there was a problem creating the output object file, an error is reported.
 */
void save_memory_to_file(char *filename){
    int i;
    FILE *output = fopen(filename, "w");
    if (output){
        char temp_str[3] = "";    
        fprintf(output, "%s\t", convert_int_to_awkward_base(IC, temp_str));
        fprintf(output, "%s", convert_int_to_awkward_base(DC, temp_str));    
        for (i = 0; i < IC; i++){
            fprintf(output, "\n%s\t", convert_int_to_awkward_base(C + i, temp_str));
            fprintf(output, "%s", convert_to_awkward_base(instructions_array[i], temp_str));
        }
        for (i = 0; i < DC ; i++){
            fprintf(output, "\n%s\t", convert_int_to_awkward_base(C + i + IC, temp_str));
            fprintf(output, "%s", convert_to_awkward_base(data_array[i], temp_str));       
        }        
        fclose(output);       
    }
    else fprintf(stderr, "Error: unable to create the file \"%s\".\n", filename);
}

/*
 * get_memory_full_flag:
 * returns the memory status flag.
 */
int get_memmory_full_flag(void){
    return memory_full_flag;
}