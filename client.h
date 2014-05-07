/* * * * * * * * * * * * * * * * * * * * * * 
 *  Filename: client.h
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014           
 * * * * * * * * * * * * * * * * * * * * * */

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/type.h>
#include <sys/socket.h>
#include <netdb.h>
#include <errno.h>
#include <signal.h>

//function declarations
void    print_usage  ();
void    get_input    (char *input);
