INCLUDE = ./includes

CFLAGS = -I$(INCLUDE)

all: server

server: Server.o
	gcc -o server Server.c

run: server
	./server 8080

clean:
	rm -f Server.o server