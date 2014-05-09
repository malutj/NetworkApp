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
#include "net_shared.h"

#define DEBUG_ON 1

//function declarations
void    print_usage			(void);
void    bind_socket			(int socket, char *port);
int		accept_connection	(int socket);
void    *session_handler	(void *);
