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

static int passive_con;
static pthread_t threads[BACKLOG];
int data_con;

int main(int argc, char **argv){
    //variables
	char *port;
    int i;
	struct addrinfo socket_in;
	int len;
	len = sizeof(socket_in);

    //handle SIGINT signal
	signal(SIGINT, sig_handler);

    //get command line arguments
    if(argc != 2) print_usage();
    port = argv[1];
    
    //create passive socket for listening
    passive_con = get_socket();

    //bind to port
    bind_socket(passive_con, port);

    //listen for connections
    start_listening(passive_con);
	printf("Server listening on port %s...\n", port);

    //loop
    while(1){
		
        //accept incoming connection
        thread_data *td = malloc(sizeof(thread_data));
        td->control_con = accept_connection(passive_con);
		printf("Server accepted client connection...\n");

        //create a new thread to handle this connection
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
* Desc: Function to handle SIG_INT
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void sig_handler(int sig){
	printf("\nReceived SIG_INT Signal...Exiting program\n");
	close(passive_con);
	exit(0);
}

/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Function called on new thread. Handles individual sessions
* Param: void *ptr - pointer to a thread_data struct
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void *session_handler(void *ptr){
	thread_data td;
	int control_con;
	char *prompt;
	int cmd;
	char msg[BUF_SIZE];
	char opts[MAX_OPT][BUF_SIZE];
	
	td = *(thread_data *)ptr;
	control_con = td.control_con;
	prompt = "Welcome to Jason' Networking App\n"
		"Command Options:\n"
		"\tlist - list contents in current directory\n"
		"\tget <filename or path> - return a specified file\n"
		"\tcd - change the current directory\n"
		"\thelp - print command list\n";

	send_msg(control_con, prompt, strlen(prompt));
	while(1){
		//receive the request
		get_msg(control_con, msg);
		if ((cmd = parse_msg(msg, opts)) == EXIT) break;

		//handle the request
		handle_request(control_con, cmd, msg);

		//send the response
		
	}

	//set thread number to zero and kill thread
	printf("Client closed the connection...terminating session.\n");
	close(control_con);
	threads[td.thread_number] = 0;
	pthread_exit(NULL);
}


/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Handles the requests coming in from the client
* Param: int control_con - the control connection socket
* Param: int cmd - the command that was requests
* Param: char *msg - the message that came in from the client
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void handle_request(int control_con, int cmd, char *msg){
	if(cmd == LIST) handleListRequest(control_con);
	else if(cmd == GET) handleGetRequest(msg);
	else if(cmd == CD) handleChangeRequest(msg);
	else if(cmd == HELP) handleHelpRequest(msg);
}

/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Handles the LIST request
* Param: int control_con - the control connection
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void handleListRequest(int control_con){
	DIR *dir;
	struct dirent *entry;
	int count, len, i;
	int num_written;
	char **temp_list;
	char *result;
	char hostname[BUF_SIZE];
	struct sockaddr *addr;
	int addr_size;
	
	//open the current directory
	if((dir = opendir(".")) == NULL) print_error(strerror(errno));
	
	//allocate memory to hold the result
	temp_list = malloc(sizeof(char *) * 10000);
	
	//start reading from the directory list
	count = len = 0;
	while((entry = readdir(dir)) != NULL){
		//create a new list entry
		temp_list[count] = malloc(sizeof(char) * 256);
		strcpy(temp_list[count], entry->d_name);
		len += (strlen(entry->d_name)+1);
		count++;
	}
	
	//send the ok message
	send_msg(control_con, "ok", 2);
	
	//get client's host name
	addr_size = sizeof(struct sockaddr);
	if(getpeername(control_con, addr, &addr_size) == -1) print_error(strerror(errno));
	if(getnameinfo(addr, &add_size, hostname, BUF_SIZE, NULL, 0, NI_NAMEREQD | NI_NUMERICHOST) == -1) print_error(strerror(errno));
	
	//connect to the data socket
	data_con = connect_data(host, DATA_PORT);
	
	//create the return string
	result = malloc(sizeof(char) * len);
	num_written = 0;
	for(i = 0; i<count; i++){
		//copy the filename into the result string
		memcpy(result+num_written,temp_list[i], strlen(temp_list[i]));
		num_written += strlen(temp_list[i]);
		//add a newline character at the end
		result[num_written] = '\n';
		num_written++;
		//free the old filename pointer
		free(temp_list[i]);
	}
	
	//free the old list
	free(temp_list);
	
	//send the result string on the data connection
	send_msg(data_con, result, num_written);
	
	//close the data connection
	close(data_con);
	
	//free the message memory
	free(result);
	
}


/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Connects to the client's data socket
* Param: char *host - the host name of the client computer
* Param: char *port - the port number to connect to
* Return: int - the file descriptor for the connection
* * * * * * * * * * * * * * * * * * * * * * * */
int connect_data(char *host, char *port){
	int fd;
	
	fd = get_socket();					//get a new socket
	make_connection(fd, host, port);	//make the connection
	return fd;	//return the socket
}
