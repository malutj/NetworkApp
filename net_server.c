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
	if(bind_socket(passive_con, port) == -1){
		printf("Sorry, port number %s is already in use. Please enter a new port number\n", port);
		exit(EXIT_FAILURE);
	}

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
	int i;
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
	char *msg;
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
		printf("Getting message from client\n");
		msg = get_msg(control_con);
		if ((cmd = parse_msg(msg, opts)) == EXIT) break;

		//handle the request
		printf("Handling the request\n");
		handle_request(control_con, cmd, msg);
		
		free(msg);
	}

	//set thread number to zero and kill thread
	printf("Client done...terminating session.\n");
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
	else if(cmd == GET) handleGetRequest(control_con, msg);
	else if(cmd == CD) handleChangeRequest(control_con);
	else if(cmd == HELP) handleHelpRequest(control_con);
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
	int data_con;
	char **temp_list;
	char *result;
	char hostname[BUF_SIZE];
	struct sockaddr_in addr;
	char *port;
	socklen_t addr_size;
	
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
	addr_size = sizeof(addr);
	if(getpeername(control_con, (struct sockaddr *)&addr, &addr_size) == -1) print_error(strerror(errno));
	if(getnameinfo((struct sockaddr *)&addr, addr_size, hostname, BUF_SIZE, NULL, 0, 0) == -1) print_error(strerror(errno));
	
	//get the port number the client is listening on
	port = get_msg(control_con);

	//connect to the data socket
	data_con = connect_data(hostname, port);
	
	//create the return string
	result = malloc(sizeof(char) * (len));
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
	result[num_written-1] = '\0';
	num_written--;
	
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
* Desc: Handles the GET request
* Param: int control_con - the control connection
* Param: char *msg - the message sent by the client
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void handleGetRequest(int control_con, char *msg){
    char filename[1][BUF_SIZE];
    char *get_error;
    int num_read;
    char c;
    int fd;
    int data_con;
    int file_size;
    char hostname[BUF_SIZE];
	struct sockaddr_in addr;
	char *port;
	socklen_t addr_size;
    
    //get the filename requested
    parse_msg(msg, filename);
    
    //make sure we're able to open the file
    if((fd=open(filename[0], O_RDONLY)) == -1){
        //error
        get_error = strerror(errno);
        send_msg(control_con, get_error, strlen(get_error));
        return;
    }
    
    //send the ok message along the control connection
    send_msg(control_con,"ok",2);
    
    //get client's host name
	addr_size = sizeof(addr);
	if(getpeername(control_con, (struct sockaddr *)&addr, &addr_size) == -1) print_error(strerror(errno));
	if(getnameinfo((struct sockaddr *)&addr, addr_size, hostname, BUF_SIZE, NULL, 0, 0) == -1) print_error(strerror(errno));
	
	//get the port number the client is listening on
	port = get_msg(control_con);
    
	//connect to the data socket
	data_con = connect_data(hostname, port);
    
    //send the filename
    send_msg(data_con, filename[0], strlen(filename[0]));
    
    //send the file size
    lseek(fd, 0, SEEK_END);
    file_size = lseek(fd, 0, SEEK_CUR);
    lseek(fd, 0, SEEK_SET);
    write(data_con, &file_size, sizeof(int));
    
    //read from the rile
    while((num_read = read(fd, &c, 1)) > 0){
        //write to the data connection
        write(data_con, &c, 1);
    }
    //error reading from file
    if(num_read == -1){
        close(data_con);
        print_error(strerror(errno));
    }
    
    //close the file and socket
    close(fd);
    close(data_con);
}

/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Handles the CD request
* Param: int control_con - the control connection
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void handleChangeRequest(int control_con){
    
}

/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Handles the HELP request
* Param: int control_con - the control connection
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void handleHelpRequest(int control_con){
    char *prompt = "Command Options:\n"
    "\tlist - list contents in current directory\n"
    "\tget <filename or path> - return a specified file\n"
    "\tcd - change the current directory\n"
    "\thelp - print command list\n";
    
    send_msg(control_con, prompt, strlen(prompt));
}

/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Connects to the client's data socket
* Param: char *host - the host name of the client computer
* Param: char *port - the port number to connect to
* Return: int - the file descriptor for the connection
* * * * * * * * * * * * * * * * * * * * * * * */
int connect_data(char *host, char *port){
	int fd;

	printf("Creating the socket for the data connection\n");
	fd = get_socket();					//get a new socket
	printf("Making the connection\n");
	make_connection(fd, host, port);	//make the connection
	printf("Connected\n");

	return fd;	//return the socket
}
