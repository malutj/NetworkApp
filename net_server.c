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
	printf("Received SIG_INT Signal...Exiting program\n");
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
	char prompt[BUF_SIZE];
	
	td = *(thread_data *)ptr;
	control_con = td.control_con;
	sprintf(prompt, "Welcome to Jason's Networking App\n\
					\tThread number %ld\n~", (long)pthread_self());
	send_msg(control_con, prompt);





	//set thread number to zero and kill thread
	threads[td.thread_number] = 0;

	pthread_exit(NULL);
}

