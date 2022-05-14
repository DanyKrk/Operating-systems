CC = gcc -Wall

all: main, deliverer, baker
	make main
	make deliverer
	make baker
test:
	make clean
	make all
	./main 3 3
main:
	$(CC) main.c -o main

deliverer:
	$(CC) deliverer.c -o deliverer

baker:
	$(CC) baker.c -o baker



clean:
	rm -f main