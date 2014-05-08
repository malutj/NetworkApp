/* * * * * * * * * * * * * * * * * * * * * *
 *  Filename: net_shared.c
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014
 *  Desc: Contains functions shared by both
 *        the client and server programs
 * * * * * * * * * * * * * * * * * * * * * */
 
#include "net_shared.h"


/* * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Prints error messages
 *  Param: char *msg - the error message to print
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * */
void print_error(char *msg){
    fprintf(stderr, "ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Create a new socket
 *  Return: int - the file descriptor for the new socket
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int get_socket(void){
    int fd;
    if((fd = socket(PF_INET, SOCK_STREAM, 0) == -1){
        print_error(strerror(errno));
    }
    return fd;
}
