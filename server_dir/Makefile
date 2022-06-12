INCLUDE = ../includes

CFLAGS = -I$(INCLUDE)

all: server

server: Server.o
	gcc -I ../includes/ -pthread -o dataServer Server.c ../queue/ADTQueue.c

run: server
	./dataServer -p 8080 -s 10 -q 10 -b 10

clean:
	rm -f Server.o dataServer