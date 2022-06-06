INCLUDE = ./includes

CFLAGS = -I$(INCLUDE)

all: client

client: Client.o
	gcc -o client Client.c

clean:
	rm -f Client.o client