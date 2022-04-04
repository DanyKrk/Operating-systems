CC = gcc
CFLAGS = -Wall

test:
	make main
	./main .. "abba" 2

explore_stat:
	$(CC) $(CFLAGS) -c explore_stat.c -o explore_stat.o

main: explore_stat
		$(CC) $(CFLAGS) main.c explore_stat.o -o main

clean:
	rm -f *.o *.a *.so 
