CFLAGS = -Wall -Wno-unused-variable -fpic -Wno-unused-but-set-variable -coverage -lm
PROGS = net_client net_server

default: all

all: client server

net_client: net_client.o net_shared.o
	@gcc -o net_client net_client.o net_shared.o $(CFLAGS)

net_client.o: net_client.h net_shared.h
	@echo "Compiling Client..."
	@gcc -c net_client.c $(CFLAGS)

net_server: net_server.o net_shared.o
	@gcc -o net_server net_server.o net_shared.o $(CFLAGS)

net_server.o: net_server.h net_shared.o
	@echo "Compiling Server..."
	@gcc -c net_server.c $(CFLAGS)

net_shared.o: net_shared.h
	@gcc -c net_shared.c $(CFLAGS)

clean:
	@echo "Cleaning up..."
	@rm -f $(PROGS) *.o


