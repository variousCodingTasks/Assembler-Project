#include "word.h"

/*
 * This module implements a "word" for the assembler's use:  a word (defined in
 * the header), contains a signed 10 bit integer to hold all the possible values
 * for the imaginary processor's use. this implementation allows negative numbers
 * to be represented in the two's complements method without using complicated
 * functions to translate an process values. the module contains the functions
 * that translate words to the "awkward base".
 */

/*this array, visible to this file only, contains the characters encoding in the "awkward base".*/
static const char encoding[] = {'!','@','#','$','%','^','&','*','<','>','a','b','c','d','e', 'f',
                                'g','h','i','j','k','l','m','n','o','p','q','r','s','t','u','v'};

/*
 * convert_to_awkward_base:
 * receives a word and a target string: converts the 10 bits integer to its
 * "awkward base" by converting each 5 bits to the corresponding character and
 * stores them in the target string. assumes the target string is at least
 * 3 characters wide.
 */
char* convert_to_awkward_base(word value, char *target){
    target[0] = encoding[(value.value&992)>>5]; /*992 in binary is 11111-00000 */
    target[1] = encoding[value.value&31]; /*31 in binary is 00000-11111*/
    target[2] = '\0';
    return target;
}

/*
 * convert_int_to_awkward_base:
 * same as "convert_to_awkward_base", but instead receives an integer,
 * it will only convert the first 10 bits in this case and ignore the 
 * rest.
 */
char *convert_int_to_awkward_base(int value, char *target){
    word temp_word;
    temp_word.value = value;
    convert_to_awkward_base(temp_word, target);
    return target;
}

/*
 * print_word_ro_binary:
 * used for debugging purposes: takes a word and prints its binary
 * representation on the screen (stdout).
 */
void print_word_to_binary(word item){
    int i, bit, value = item.value;
    for (i = 9; i >= 0; i--){
        bit = (value>>i) & 01;
        printf("%d",bit);
    }
    printf(" ");
}