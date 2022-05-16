CC = gcc -Wall

all: main deliverer baker
	make main
	make deliverer
	make baker
test: clean all
	./main 1 1

main:
	$(CC) main.c -o main -lpthread -lrt

deliverer:
	$(CC) deliverer.c -o deliverer -lpthread -lrt

baker:
	$(CC) baker.c -o baker -lpthread -lrt



clean:
	rm -f main baker deliverer