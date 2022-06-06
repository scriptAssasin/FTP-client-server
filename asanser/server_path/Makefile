INCLUDE = ./includes

CFLAGS = -I$(INCLUDE)

all: server

server: Server.o
	gcc -o server Server.c

clean:
	rm -f Server.o server