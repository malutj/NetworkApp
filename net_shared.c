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
	if ((status = getaddrinfo(NULL, port, &hints, &result)) != 0){
		close(socket);
		printf("Error: %s\n",gai_strerror(status));
		exit(EXIT_FAILURE);
	}

	//bind the socket
	if ((bind(socket, result->ai_addr, result->ai_addrlen)) == -1){
		close(socket);
		freeaddrinfo(result);
		print_error(strerror(errno));
	}

	//free the linked list
	freeaddrinfo(result);
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


/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Accepts an incoming connection
* Param: int socket - the socket to use
* Return: a file descriptor for the new connection
* * * * * * * * * * * * * * * * * * * * * * * */
int accept_connection(int socket){
	struct sockaddr_in storage;
	int new_socket;
	socklen_t addrlen;

	addrlen = sizeof(storage);
	if ((new_socket = accept(socket, (struct sockaddr *)&storage, &addrlen)) == -1){
		close(socket);
		print_error(strerror(errno));
	}
	return new_socket;
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Desc: Create a new connection
*  Param: int socket - the socket file descriptor we'll be using
*  Param: char *host - the host name or IP number of for the connection
*  Param: char *port - the port number for the connection
*  Return: void
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void make_connection(int socket, char *host, char *port){
	struct addrinfo hints, *result, *p;
	int status;

	//set up the hints struct
	memset(&hints, 0, sizeof(struct addrinfo));
	hints.ai_family = AF_INET;
	hints.ai_socktype = SOCK_STREAM;

	//get address info
	if ((status = getaddrinfo(host, port, &hints, &result)) != 0){
		close(socket);
		printf("Error: %s\n", gai_strerror(status));
		exit(EXIT_FAILURE);
	}

	//connect to the server
	for (p = result; p != NULL; p = p->ai_next){

		//if connection is successful
		if ((status = connect(socket, p->ai_addr, p->ai_addrlen)) != -1){

			//free the linked list
			freeaddrinfo(result);

			return;
		}
	}

	//unable to connect; free linked list and close socket before exit
	freeaddrinfo(result);
	close(socket);
	print_error("Unable to create connection to the server");
}


/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Desc: Sends a message over a socket
*  Param: int fd - the socket on which to send the message
*  Param: char *msg - the socket on which to send the message
*  Param: int msg_size - the number of bytes in the message
*  Return: int - the file descriptor for the new socket
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void send_msg(int fd, char *msg, int msg_size){

	//first send the size of the message
	if(write(fd, msg_size, sizeof(int)) == -1){
		close(fd);
		print_error(strerror(errno));
	}
	
	//now send the message itself
	if (write(fd, msg, strlen(msg)) == -1){
		close(fd);
		print_error(strerror(errno));
	}
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Desc: Reads message from the control connection
*  Param: char *msg - the string we'll store the message in
*  Return: int - the socket to read from
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void get_msg(int fd, char *msg){
	int num_read;
	int msg_size;
	int i;

	//first get the size of the message
	if((num_read = recv(fd, &msg_size, sizeof(int), 0)) == -1) print_error(strerror(errno));
	
	//read the message
	i = 0;
	while(i < msg_size){
		//error reading from socket
		if ((num_read = recv(fd, &msg[i], 1, 0)) == -1) print_error(strerror(errno));
		
		//socket was closed
		if (num_read == 0){
			printf("Control connection was closed unexpectedly. Exiting program\n");
			close(fd);
			exit(EXIT_FAILURE);
		}
		i+=num_read;
	}
	msg[msg_size] = '\0';
	
	
	//below is my first pass at reading a message
	/*
	i = 0;
	do{
		//error reading from socket
		if ((num_read = recv(fd, &msg[i], 1, 0)) == -1) print_error(strerror(errno));

		//socket was closed
		if (num_read == 0){
			printf("Control connection was closed unexpectedly. Exiting program\n");
			close(fd);
			exit(EXIT_FAILURE);
		}
		if (msg[i] == MSG_END) break;
		i += num_read;
	} while (i < BUF_SIZE);
	msg[i] = '\0';
	*/
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * *
*  Desc: Parses the message
*  Param: char msg[] - the message to parse
*  Param: char *opts[] - the array to store any options that were provided
*  Return: int - the command that was received
* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int parse_msg(char msg[], char opts[][BUF_SIZE]){
	char cmd_string[BUF_SIZE];
	int i, cmd, opt_start, opt_num;


	//copy the command into the cmd_string variable
	for (i = 0; i < BUF_SIZE; i++){
		if (msg[i] == ' ' || msg[i] == '\n' || msg[i] == MSG_END) break;
	}
	memcpy(cmd_string, msg, i);
	cmd_string[i] = '\0';
	opt_start = i + 1;        //arg_start holds the beginning of the argument if there is one

	// determine what command was given //
	//----------------------------------//
	//EXIT
	if (strcmp(cmd_string, "exit") == 0){ cmd = EXIT; }

	//LIST
	else if (strcmp(cmd_string, "list") == 0){ cmd = LIST; }

	//GET
	else if (strcmp(cmd_string, "get") == 0){ cmd = GET; }

	//NOT RECOGNIZED
	else{
		//print error message
		printf("Command '%s' is not recognized. Type 'help' to get a list of commands\n", cmd_string);
		cmd = INPUT_ERROR;
	}

	//populate the option array
	opt_num = 0;

	//make sure an argument was provided if required//
	//----------------------------------------------//
	/*if (cmd == GET){

		//find the end of the argument string
		for (i = arg_start; i < BUF_SIZE; i++){
			if (input[i] == '\n' || input[i] == ' ') break;
		}

		//make sure the argument string isn't blank
		if (i - arg_start == 0){
			printf("The 'get' command requires a <filename> argument\n");
			return INPUT_ERROR;
		}
	}*/


	return cmd;
}
