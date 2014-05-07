/* * * * * * * * * * * * * * * * * * * * * *
*   Filename: client.c
*   Owner: Jason M Malutich
*   Date: 7 May 2014
* * * * * * * * * * * * * * * * * * * * * */

#include "client.h"


int main(int argc, char **argv){
    
    //variables
    char *host;
    char input[BUF_SIZE];
    int port;
    int controlCon;
    
    
    
    //get command line arguments
    if(argc != 3) printUsage();
    host = argv[1];
    port = atoi(argv[2]);
    
    //create socket for control connection
    //connect to host on specified port number
    //enter loop
    do{
        
    }while(getInput() != "exit");
    
    
    
    return 0;
}


/* * * * * * * * * * * * * * * * * * * * * * * * 
 * Desc: Prints usage instructions for program
 * Return: void
 * * * * * * * * * * * * * * * * * * * * * * * */
 void printUsage(){
     puts("Usage: client <hostname> <port number>\n");
     exit(0);
 }
