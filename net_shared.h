/* * * * * * * * * * * * * * * * * * * * * *
 *  Filename: net_shared.h
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014
 *  Desc: Contains functions shared by both
 *        the client and server programs
 * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <errno.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>

#define DEBUG(MSG) if(DEBUG_ON) printf("%s",MSG)

#define INPUT_ERROR -1
#define EXIT 0
#define LIST 1
#define GET 2
#define CD 3
#define HELP 4

#define BUF_SIZE 1000
#define MAX_OPT 10
#define BACKLOG 10
#define DATA_PORT "98125"



//shared function declarations
void print_error(char *msg);
int get_socket();
int bind_socket(int socket, char *port);
void start_listening(int fd);
int accept_connection(int socket);
void make_connection(int socket, char *host, char *port);
void send_msg(int fd, char *msg, int msg_size);
char * get_msg(int fd);
int parse_msg(char msg[], char opts[][BUF_SIZE]);
