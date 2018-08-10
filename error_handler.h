#ifndef ERROR_HANDLER_H
#define ERROR_HANDLER_H

    #include <stdio.h>
    #include <stdlib.h>
    #include <string.h>
	
    void print_error_string(int, int, char*);
    void print_error_char(int, int, char);
    void print_error(int, int);
    void print_warning_string(int, int, char*);
    void print_warning_int(int, int, int);
    void print_warning(int, int);
    void *exit_program_fatal_error(void);

#endif

