INCLUDE = ./includes

CFLAGS = -I$(INCLUDE)

all: client

client: Client.o
	gcc -lpthread -o client Client.c

run: client
	./client 127.0.0.1 8080 dummy

clean:
	rm -f Client.o client