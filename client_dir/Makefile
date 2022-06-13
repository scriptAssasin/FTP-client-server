INCLUDE = ./includes

CFLAGS = -I$(INCLUDE)

all: client

client: Client.o
	gcc -lpthread -o remoteClient Client.c

run: client
	./remoteClient -i 127.0.0.1 -p 8080 -d directory
	
clean:
	rm -f Client.o remoteClient