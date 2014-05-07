/* * * * * * * * * * * * * * * * * * * * * *
 *  Filename: client.c
 *  Owner: Jason M Malutich
 *  Date: 7 May 2014
 * * * * * * * * * * * * * * * * * * * * * */

#include "client.h"


int main(int argc, char **argv){
    
    //variables
    char *host;
    char input[OPT_COUNT][BUF_SIZE];
    int port;
    int control_con;
    
    
    
    //get command line arguments
    if(argc != 3) print_usage();
    host = argv[1];
    port = atoi(argv[2]);
    
    //create socket for control connection
    //connect to host on specified port number
    //enter loop
    do{
        
    }while(get_input() != "exit");
    
    
    
    return 0;
}


/* * * * * * * * * * * * * * * * * * * * * * * * 
 *  Desc: Prints usage instructions for program
 *  Return: void
 * * * * * * * * * * * * * * * * * * * * * * * */
void print_usage(){
    puts("Usage: client <hostname> <port number>\n");
    exit(0);
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
