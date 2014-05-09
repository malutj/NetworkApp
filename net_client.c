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
    int command;
    char input[BUF_SIZE+1];
    
	//handle SIGINT signal
	signal(SIGINT, sig_handler);

    //get command line arguments
    if(argc != 3) print_usage();
    host = argv[1];
    port = argv[2];
    
    //create socket for control connection
DEBUG(":CREATING SOCKET...");
    control_con = get_socket();

    //connect to the server on the control connection
DEBUG(":CONNECTING TO SERVER...");
    make_connection(control_con, host, port);
    
    //enter loop
    do{

        //get input from the user
DEBUG(":GETTING INPUT FROM USER...");
        if((command = get_user_input(input)) == INPUT_ERROR) continue;
        
        //send command to the server
DEBUG(":SENDING COMMAND TO THE SERVER...");
	

    }while(command != EXIT);
    
    
    
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
 *  Return: int - the command that was given
 * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * * */
int get_user_input(char input[]){
	char opts[MAX_OPT][BUF_SIZE];
    int i, cmd;
    
    //read from command line into input array
    fgets(input, BUF_SIZE, stdin);

	//validate the user's input
	cmd = parse_msg(input, opts);

	return cmd;
}


/* * * * * * * * * * * * * * * * * * * * * * * *
* Desc: Function to handle SIG_INT
* Return: void
* * * * * * * * * * * * * * * * * * * * * * * */
void sig_handler(int sig){
	printf("Received SIG_INT Signal...Exiting program\n");
	close(control_con);
	exit(0);
}
