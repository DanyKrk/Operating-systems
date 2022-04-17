CC = gcc
CFLAGS = -Wall
ARGS = arguments.txt

test:
	make main
	./main $(ARGS)

main:
	$(CC) $(CFLAGS) main.c -o main

clean:
	rm -f *.o *.a *.so 

valgrind:
	make main
	valgrind ./main $(ARGS)
