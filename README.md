# NetworkApp

This program is a 2-connection client-server network application that connects via a control connection and sends responses over a data connection.  
The server is multi-threaded, so it can accept numerous client connections at one time.  
Both the client and the server catch `SIGINT` signals and shut down any open sockets, so feel free to kill the programs at any time.


# Usage


### Makefile
To compile and link both the client and server, simply type  
   
`make all` 
   
To clean up the files type  
   
`make clean`
   
### Server
To start the server, type  
 
`./net_serve <port>`
 
To shutdown the server, type  
 
`ctrl+c` and the code will catch the SIGINT and close out the open sockets
 
### Client
To start the client, type  
 
`./net_client <host> <port>`
 
##### Commands
* list - returns a listing of all files in the server's working directory 
* get <filename> - copies a file from the server's directory into the client's directory   
     *If the file already exists in the client's directory, the program will ask if you want to overwrite the file* 
* help - prints out a list of the commands 
* exit - closes the connection with the server (does not shut down the server) 
