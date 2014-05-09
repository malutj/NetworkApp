/* * * * * * * * * * * * * * * * * * * * * *
*   Filename: net_server.c
*   Owner: Jason M Malutich
*   Date: 7 May 2014
* * * * * * * * * * * * * * * * * * * * * */

#include "net_server.h"

typedef struct thread_data{
    int thread_number;
    int control_con;
} thread_data;


int main(int argc, char **argv){
    //variables
	char *port;
	int passive_con;
    pthread_t threads[BACKLOG];
    int i;
    //need to set up signals
    
    //get command line arguments
    if(argc != 2) print_usage();
    port = argv[1];
    
    //create passive socket for listening
DEBUG(":CREATING SOCKET...");
    passive_con = get_socket();
DEBUG("SOCKET CREATED\n");

    //bind to port
DEBUG(":BINDING TO PORT...");
    bind_socket(passive_con, port);
DEBUG("BOUND\n");

    //listen for connections
DEBUG(":STARTING TO LISTEN FOR CONNECTIONS...");
    bind_socket(passive_con, port);
DEBUG("LISTENING\n");

    //loop
    while(1){
        
        //accept incoming connection
        thread_data *td = malloc(sizeof(thread_data));
DEBUG(":ACCEPTING INCOMING CONNECTIONS...");
        td->control_con = accept_connection(passive_con);
DEBUG("CONNECTION ACCEPTED\n");

        //create a new thread to handle this connection
DEBUG(":CREATING A NEW THREAD...");
        //figure out the next open thread number
for (i = 0; i < BACKLOG; i++){
            if(threads[i] == 0) break;
        }
        td->thread_number = i;
        
        //create the new thread
        if(pthread_create(&threads[i], NULL, session_handler, td) == -1){
            close(passive_con);
            close(td->control_con);
            free(td);
            print_error(strerror(errno));
        }
    }
    
    
    return 0;
}

/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Prints usage instructions for program
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void print_usage(void){
    puts("Usage: net_server <port number>\n");
    exit(EXIT_FAILURE);
}

/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Binds the socket to a local port
* Param: int socket - the socket to use
* Param: char *port - the port to bind to
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void bind_socket(int socket, char *port){
    struct addrinfo hints, *result;
    int status;
    
    //set up the hints struct
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    hints.ai_flags = AI_PASSIVE;        //auto-populate my IP address
    
    //get address info
    if((status = getaddrinfo(NULL, port, &hints, &result)) != 0){
        close(socket);
        print_error(gai_strerror(status));
    }
    
    //bind the socket
    if((bind(socket, result->ai_addr, result->ai_addrlen)) == -1){
        close(socket);
        freeaddrinfo(result);
        print_error(strerror(errno));
    }
    
    //free the linked list
    freeaddrinfo(result);
}


/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Accepts an incoming connection
* Param: int socket - the socket to use
* Return: a file descriptor for the new connection
* * * * * * * * * * * * * * * * * * * * * * * */
int accept_connection(int socket){
    struct sockaddr_storage storage;
    int new_socket;
    socklen_t addrlen;

    addrlen = sizeof storage;
	if ((new_socket = accept(socket, (struct sockaddr *)&storage, &addrlen)) == -1){
        close(socket);
        print_error(strerror(errno));
    }
    return new_socket;
}


/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Function called on new thread. Handles individual sessions
* Param: void *ptr - pointer to a thread_data struct
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void *session_handler(void *ptr){
    thread_data *td = (thread_data *)ptr;

	return NULL;
}
