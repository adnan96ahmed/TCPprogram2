CC = gcc
CFLAGS = -Wall -g

all: client server runScript

client: TCPclient.c
	$(CC) $(CFLAGS) -o client TCPclient.c -lpthread
	
server: TCPserver.c queue.h
	$(CC) $(CFLAGS) -o server TCPserver.c fileQueue.c -lpthread

runScript:
	chmod a+x pythonScript.py

clean:
	rm client server