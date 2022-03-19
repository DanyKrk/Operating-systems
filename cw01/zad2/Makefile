CC = gcc
CFLAGS = -Wall
REPORT_FILE = raport2.txt
TEST_ARGS_FILE = test_args.txt

all: static shared main
	test

static: 
	$(CC) $(CFLAGS) -c library.c
	ar rcs library.a library.o
	rm -f library.o

shared: 
	$(CC) $(CFLAGS) -c -fPIC library.c
	$(CC) -shared library.o -o liblibrary.so
	rm -f library.o

main_shared: shared
	$(CC) $(CFLAGS) main.c -L. -Wl,-rpath=. -llibrary -o main

main_static: static
	$(CC) $(CFLAGS) main.c library.a -o main

main:
	make main_static

test: main
	rm -f $(REPORT_FILE)
	./main $(shell cat $(TEST_ARGS_FILE))

clean:
	rm -f *.o *.a *.so trash.txt temp.txt
