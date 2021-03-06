/* * * * * * * * * * * * * * * * * * * * * *
*   Filename: net_server.h
*   Owner: Jason M Malutich
*   Date: 7 May 2014
* * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <pthread.h>
#include <signal.h>
#include <dirent.h>
#include "net_shared.h"

#define DEBUG_ON 0

//function declarations
void print_usage(void);
void *session_handler(void *);
void sig_handler(int sig);
void handle_request(int control_con, int cmd, char msg[]);
void handleListRequest(int control_con);
void handleGetRequest(int control_con, char *msg);
void handleChangeRequest(int control_con);
void handleHelpRequest(int control_con);
int connect_data(char *host, char *port);

