CFLAGS = -Wall -Wno-unused-variable -fpic -Wno-unused-but-set-variable -lm
PROGS = net_client net_server

default: all

all: net_client net_server

net_client: net_client.o net_shared.o
	@gcc -o net_client net_client.o net_shared.o $(CFLAGS)

net_client.o: net_client.h net_client.c net_shared.o
	@echo "Compiling Client..."
	@gcc -c net_client.c $(CFLAGS)

net_server: net_server.o net_shared.o
	@gcc -o net_server net_server.o net_shared.o $(CFLAGS) -lpthread

net_server.o: net_server.h net_server.c net_shared.o
	@echo "Compiling Server..."
	@gcc -c net_server.c $(CFLAGS)

net_shared.o: net_shared.h net_shared.c
	@gcc -c net_shared.c $(CFLAGS)

clean:
	@echo "Cleaning up..."
	@rm -f $(PROGS) *.o


