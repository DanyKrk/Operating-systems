CC = gcc
CFLAGS = -Wall

all: static shared

static: 
	$(CC) $(CFLAGS) -c library.c
	ar rcs library.a library.o
	rm -f library.o

shared: 
	$(CC) $(CFLAGS) -c -fPIC library.c
	$(CC) -shared -fPIC library.o -o library.so
	rm -f library.o

clean:
	rm -f *.o *.a *.so
