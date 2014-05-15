/* * * * * * * * * * * * * * * * * * * * * *
 *  Filename: net_client.c
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014
 * * * * * * * * * * * * * * * * * * * * * */

#include "net_client.h"

static int control_con;
static int data_con;

int main(int argc, char **argv){
    
    //variables
    char *host, *port;
    int cmd;
	int msg_size;
    char input[BUF_SIZE+1];
	char msg[BUF_SIZE + 1];
	char opts[MAX_OPT][BUF_SIZE];
    
	//handle SIGINT signal
	signal(SIGINT, sig_handler);

    //get command line arguments
    if(argc != 3) print_usage();
    host = argv[1];
    port = argv[2];
    
    //create socket for control connection
    control_con = get_socket();

    //connect to the server on the control connection
    make_connection(control_con, host, port);
    
	//read initial prompt from server
	get_msg(control_con, msg);
	printf("%s", msg);

    //enter loop
    do{
		//print a little prompt
		printf("enter command>> ");
			
	    //get input from the user
		get_user_input(input, &msg_size);
		if ((cmd = parse_msg(input, opts)) == INPUT_ERROR) continue;
			
	    //send command to the server
		send_msg(control_con, input);
			
		//if command is to exit, break
		if(cmd == EXIT) break;
			
		//get success/error msg from server
		get_msg(control_con, msg);
			
		//print error message from server
		if(msg != "ok"){
			printf("%s\n", msg);
			continue;
		}
			
		//handle the response
		handle_response(cmd);

    }while(cmd != EXIT);

	//close the connection
	printf("Closing connection with server...\nExiting program...\n");
	close(control_con);
    
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

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Get the user's input
 *  Param: char input[] - string to hold the user's input
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
void get_user_input(char input[], int *msg_size){
	char opts[MAX_OPT][BUF_SIZE+1];
    int i, cmd;
    
    //read from command line into input array
    fgets(input, BUF_SIZE, stdin);
	for (i = 0; i < BUF_SIZE; i++){
		if (input[i] == '\n') break;
	}
	input[i] = '\0';
	*msg_size = i;
}

/* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Handle the response from the server
 *  Param: int fd - the data connection 
 *  Param: int cmd - the command that was sent to the server
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 void handle_response(int cmd){
	
	data_con = get_socket();			//open the socket
	bind_socket(data_con, DATA_PORT);	//bind the socket
	start_listening(data_con);			//listen on the socket
	accept_connection(data_con);		//accept connection from server
	
 	//list directory contents
 	if(cmd == LIST)	getListResponse(fd);
 	
 	//get a file
 	else if(cmd == GET) getFileResponse(fd);
 	
 	close(data_con);
 }
 
 
 /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Print the LIST response from server
 *  Param: int fd - the data connection 
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 void getListResponse(int fd){
 	int msg_size;
 	int num_read, i;
 	char *resp;
 	
 	//get the size of the response
 	num_read = i = 0;
 	while(num_read < sizeof(int)){
 		if((i=recv(fd, &msg_size[i], sizeof(int)-i, 0)) == -1) print_error(strerror(errno));
 		num_read += i;
 	}
 	
 	//allocate memory for response
 	resp = malloc(sizeof(char) * (msg_size+1));
 	
 	//get the response
 	num_read = i = 0;
 	while(num_read < msg_size){
 		if((i=recv(fd, &resp[i], msg_size[i], 0)) == -1) print_error(strerror(errno));
 		num_read+=i;
 	}
 	resp[msg_size] = '\0';
 	
 	//print the response
 	printf("%s\n\n", resp);
 	
 	//free the memory
 	free(resp);
 }
 
  /* * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Handle the GET file response
 *  Param: int fd - the data connection 
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
 void getFileResponse(int fd){
 	int fname_size;
 	int file_size;
 	int num_read, num_written, i;
 	int outfile;
 	char *filename;
 	char *file_contents;
 	char overwrite_resp[3];
 	int overwrite;
 	
 	//get size of filename
 	num_read = i = 0;
 	while(num_read < sizeof(int)){
 		if((i=recv(fd, &fname_size[i], sizeof(int)-i, 0)) == -1) print_error(strerror(errno));
 		num_read+=i;
 	}
 	
 	//allocate memory for the filename
 	filename = malloc(sizeof(char) * (fname_size+1));
 	
 	//get the filename
 	num_read = i = 0;
 	while(num_read < fname_size){
 		if((i=recv(fd, &filename[i], fname_size-i, 0)) == -1) print_error(strerror(errno));
 		num_read += i;
 	}
 	filename[fname_size] = '\0';
 	
 	//get the size of the file
 	num_read = i = 0;
 	while(num_read < sizeof(int)){
 		if((i=recv(fd, &file_size[i], sizeof(int)-i, 0)) == -1) print_error(strerror(errno));
 		num_read+=i;	
 	}
 	
 	//allocate memory for the file
 	file_contents = malloc(sizeof(char) * (file_size));
 	
 	//get the file
 	num_read = i = 0;
 	while(num_read < file_size){
 		if((i=recv(fd, &file_contents[i], file_size-i, 0)) == -1) print_error(strerror(errno));
 		num_read += i;	
 	}
 	
 	//open the file
 	if((outfile = open(filename, O_WRONLY | O_CREAT | O_EXCL)) == -1){
 		//file already exists
 		if(errno == EEXIST){
 			//ask user if they want to overwrite file
 			printf("The filename '%s' already exists in the current directory. Do you want to overwrite? 'yes' or 'no': "", filename);
 			do{
	 			fgets(overwrite_resp, 3, stdin);
	 			//yes, overwrite
	 			if(strncmp(overwrite_resp, "yes", 3) == 0){
	 				overwrite = 1;
	 				break;
	 			}
	 			//no, don't overwrite
	 			else if(strncmp(overwrite_resp, "no", 2) == 0){
	 				overwrite = 0;
	 				break;
	 			}
	 			//don't recognize input
	 			else{
	 				printf("Please type 'yes' to overwrite or 'no' to cancel: ");
	 			}
 			}while(1);
 			
 			//reopen the file
 			if(overwrite){
 				if((outfile = open(filename, O_WRONLY)) == -1) print_error(strerror(errno));
 			}
 			//cancel the write operation and return
 			else{
 				printf("File copy has been cancelled\n");
 				free(filename);
 				free(file_contents);
 				return;
 			}
 		}
 	}
 	
 	//file is open, proceed to write
 	num_written = i = 0;
 	while(num_written < file_size){
 		if((i=write(outfile, &file_contents[i], file_size-i)) == -1) print_error(strerror(errno));
 		num_written += i;
 	}
 	
 	//close the file
 	close(outfile);
 	
 	//free memory
 	free(filename);
 	free(file_contents);
 	
 	//print success message and return
 	printf("Finished copying file\n");
 }
 
/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Function to handle SIG_INT
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void sig_handler(int sig){
	printf("\nReceived SIG_INT Signal...Exiting program\n");
	close(control_con);
	close(data_con);
	exit(0);
}
