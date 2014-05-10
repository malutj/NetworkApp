/* * * * * * * * * * * * * * * * * * * * * *
 *  Filename: net_client.c
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014
 * * * * * * * * * * * * * * * * * * * * * */

#include "net_client.h"

static int control_con;

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

        //get input from the user
		get_user_input(input);
		if ((cmd = parse_msg(input, opts)) == INPUT_ERROR) continue;
		
        //send command to the server
		send_msg(control_con, input);
		
		//handle response
		

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
void get_user_input(char input[]){
	char opts[MAX_OPT][BUF_SIZE+1];
    int i, cmd;
    
    //read from command line into input array
    fgets(input, BUF_SIZE, stdin);
	for (i = 0; i < BUF_SIZE; i++){
		if (input[i] == '\n') break;
	}
	input[i] = MSG_END;
}


/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Function to handle SIG_INT
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void sig_handler(int sig){
	printf("\nReceived SIG_INT Signal...Exiting program\n");
	close(control_con);
	exit(0);
}
