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
    printf("ERROR: %s\n", msg);
    exit(EXIT_FAILURE);
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Create a new socket
 *  Return: int - the file descriptor for the new socket
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int get_socket(void){
    int fd;
    if((fd = socket(PF_INET, SOCK_STREAM, 0)) == -1){
        print_error(strerror(errno));
    }
    return fd;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Desc: Listen on the specified socket
*  Param: int fd - the socket on which to listen
*  Return: int - the file descriptor for the new socket
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void start_listening(int fd){
	if (listen(fd, BACKLOG) == -1){
		close(fd);
		print_error(strerror(errno));
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Desc: Sends a message over a socket
*  Param: int fd - the socket on which to send the message
*  Param: char *msg - the socket on which to send the message
*  Return: int - the file descriptor for the new socket
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void send_msg(int fd, char *msg){
	//int len, total, num_written;
	//len = strlen(msg);
	//total = 0;

	//while (total < len){
	if (write(fd, msg, strlen(msg)) == -1){
		close(fd);
		print_error(strerror(errno));
	}
	//}

}


