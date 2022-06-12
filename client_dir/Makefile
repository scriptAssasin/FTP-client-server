INCLUDE = ./includes

CFLAGS = -I$(INCLUDE)

all: client

client: Client.o
	gcc -lpthread -o remoteClient Client.c

run: client
	sudo ./remoteClient 127.0.0.1 8080 dummy
	# sudo ./remoteClient -i 127.0.0.1 -p 8080 -d dummy

clean:
	rm -f Client.o remoteClient