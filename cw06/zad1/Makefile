CC = gcc -Wall

all:
	make server
	make client

server:
	$(CC) server.c -o server

client:
	$(CC) client.c -o client

clean:
	rm -f client server server_log.txt
