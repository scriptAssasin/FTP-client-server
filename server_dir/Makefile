INCLUDE = ../includes

CFLAGS = -I$(INCLUDE)

all: server

server: Server.o
	gcc -I ../includes/ -pthread -o server Server.c ../queue/ADTQueue.c threadpool.c

run: server
	./server 8080 10 10

clean:
	rm -f Server.o server