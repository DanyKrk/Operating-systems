CC = gcc
CFLAGS = -Wall -g

test: clean compile
	./main

compile:
	$(CC) $(FLAGS) main.c -o main -lpthread

clean:
	rm -f main