#ifndef WORD_H
#define WORD_H

    #include <stdio.h>
    #include <stdlib.h>
	
    /*a struct that contains a 10 bit signed integer*/
    typedef struct word {
        int value : 10;
    } word;
    
    char *convert_to_awkward_base(word, char*);
    char *convert_int_to_awkward_base(int, char*);
    void print_word_ro_binary(word);
    
#endif