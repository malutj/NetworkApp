/* * * * * * * * * * * * * * * * * * * * * *
 *  Filename: net_client.c
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014
 * * * * * * * * * * * * * * * * * * * * * */

#include "net_client.h"


int main(int argc, char **argv){
    
    //variables
    char *host, *port;
    char input[OPT_COUNT][BUF_SIZE];
    int control_con;
    
    
    //get command line arguments
    if(argc != 3) print_usage();
    host = argv[1];
    port = argv[2];
    
    //create socket for control connection
    control_con = get_socket();
    
    //connect to the server on the control connection
    get_connection(control_con, host, port);
    
    //enter loop
    do{
        
        //get message from server
        receive_msg(control_con);
    }while(get_input() != "exit");
    
    
    
    return 0;
}


/* * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Prints usage instructions for program
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * */
void print_usage(void){
    puts("Usage: client <hostname> <port number>\n");
    exit(EXIT_FAILURE);
}

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
 *  Desc: Get the user's input commands
 *  Param: char *input - pointer where we'll store the user's input
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void get_input(char **input){
    //read from command line into input array
    fgets(input[0], BUF_SIZE, STD_IN);
     
    //determine the command
    //LIST
    if(strncmp(input[0], "ls", 2) == 0){
         
    }
    
    //GET
    else if(strncmp(input[0], "get", 3) == 0){
        
    }
    
    //NOT RECOGNIZED
    else{
        //print command list
    }
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
 
/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Create a new connection
 *  Param: int socket - the socket file descriptor we'll be using
 *  Param: char *host - the host name or IP number of for the connection
 *  Param: char *port - the port number for the connection
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void get_connection(int socket, char *host, char *port){
    struct addrinfo hints, *result, *p;
    int status;
    int p;
    
    //set up the hints struct
    memset(&hints, 0, sizeof(struct addrinfo));
    hints.ai_family = AF_INET;
    hints.ai_socktype = SOCK_STREAM;
    
    //get address info
    if((status = statgetaddrinfo(host, port, &hints, &result) != 0){
        print_error(gai_strerror(status));
    }
    
    //connect to the server
    for(p=result; p != NULL; p = p->ai_next){
        
        //if connection is successful
        if((status = connect(socket, p->ai_addr, p->ai_addrlen)) != -1){
            
            //free the linked list
            freeaddrinfo(result);
            
            //read initial prompt from server
            receive_msg(socket);
            
            return;
        }
    }
    
    //unable to connect; free linked list and close socket before exit
    freeaddrinfo(result);
    close(socket);
    print_error("Unable to create connection to the server");
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Reads message from the control connection
 *  Return: int - the socket to read from
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void receive_msg(int fd){
    char msg[BUF_SIZE+1], msg_size[MSG_SIZE_BUF + 1];
    int num_read;
    int msg_bytes, i;
    
    i = 0;
    
    //read first MSG_SIZE_BUF bytes of msg to get the size of the incoming message
    do{
        //error reading from socket
        if((num_read = rec(fd, &msg_size[i], MSG_SIZE_BUF-i)) == -1) print_error(strerror(errno));
        
        //socket was closed by server
        if(num_read == 0){
            close(fd);
            puts("Server has closed the connection...exiting program");
            exit(0);
        }
        i += num_read;
    }while(i < 4);
    
    //set the msg_length
    msg_size[MSG_SIZE_BUF] = '\0';
    msg_bytes = atoi(msg_size);
    
    //read the message
    i=0;
    do{
        //error reading from socket
        if((num_read = rec(fd, &msg[i], SIZE_BUF - 1)) == -1) print_error(strerror(errno));
        
        //socket was closed by server
        if(num_read == 0){
            close(fd);
            puts("Server has closed the connection...exiting program");
            exit(0);
        }
        
        i += num_read;
    }while(i < msg_size)
    
    //print the message
    printf("%s", msg);
    return;
}
