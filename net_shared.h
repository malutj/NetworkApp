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
#include <errno.h>
#include <sys/socket.h>

#define DEBUG(MSG) if(DEBUG_ON) printf("%s",MSG)

#define EXIT 0
#define LIST 1
#define GET 2
#define CD 3
#define BUF_SIZE 1000
#define MSG_SIZE_BUF 4
#define BACKLOG 10

//shared function declarations
void    print_error     (char *msg);
int     get_socket      (void);
