CC = gcc
CFLAGS = -Wall
ARG = sender

test:
	make total_clean
	make main
	./main $(ARG)

main:
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f *.o *.a *.so 

total_clean:
	make clean
	rm -f main