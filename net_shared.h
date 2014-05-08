/* * * * * * * * * * * * * * * * * * * * * *
 *  Filename: net_shared.h
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014
 *  Desc: Contains functions shared by both
 *        the client and server programs
 * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>

#define DEBUG(MSG) DEBUG_ON ? printf("%s",MSG) : "";

//define statements
//commands
#define EXIT 0
#define LIST 1
#define GET 2
#define CD 3

//buffer sizes
#define BUF_SIZE 1000
#define MSG_SIZE_BUF 4

//shared function declarations
void    print_error     (char *msg);
int     get_socket      (void);
