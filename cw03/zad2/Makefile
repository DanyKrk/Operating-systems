CC = gcc
CFLAGS = -Wall


test: 
	make main
	./main 0.2 3

main:
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f *.o *.a *.so w*
