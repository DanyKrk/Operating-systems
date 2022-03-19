CC = gcc
CFLAGS = -Wall
REPORT_FILE = results3a.txt
TEST_ARGS_FILE = test_args.txt

all: 
	test_all

static: 
	$(CC) $(CFLAGS) -c library.c
	ar rcs library.a library.o
	rm -f library.o

main_static: static
	$(CC) $(CFLAGS) main.c library.a -L. -o main

shared: 
	$(CC) $(CFLAGS) -c -fPIC library.c
	$(CC) -shared library.o -o liblibrary.so
	rm -f library.o

main_shared: shared
	$(CC) $(CFLAGS) main.c -L. -Wl,-rpath=. -llibrary -o main

main_dynamic: shared
	$(CC) $(CFLAGS) main.c -ldl -D DYNAMIC -o main

test:
	./main $(shell cat $(TEST_ARGS_FILE))

test_all:
	rm -f $(REPORT_FILE)
	printf "\nstatic: \n" >> $(REPORT_FILE)
	make main_static
	make test
	make clean
	printf "\nshared: \n" >> $(REPORT_FILE)
	make main_shared
	make test
	make clean
	printf "\ndynamic: \n" >> $(REPORT_FILE)
	make main_dynamic
	make test
	make clean


clean:
	rm -f *.o *.a *.so trash.txt temp.txt
