/* * * * * * * * * * * * * * * * * * * * * * 
 *  Filename: net_client.h
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014           
 * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>
#include "net_shared.h"

#define DEBUG_ON 1

//function declarations
void    print_usage     (void);
void    print_error     (char *msg);
void    get_input       (int *cmd, char *arg);
int     get_socket      (void);
void    get_connection  (int socket, char *host, char *port);
void    receive_msg     (int fd);
