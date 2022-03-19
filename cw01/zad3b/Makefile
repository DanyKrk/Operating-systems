CC = gcc
CFLAGS = -Wall
REPORT_FILE = results3b.txt
TEST_ARGS_FILE = test_args.txt

all: 
	test_all

static: 
	$(CC) $(CFLAGS) -c library.c
	ar rcs library.a library.o
	rm -f library.o

main_static: static
	$(CC) $(CFLAGS) main.c library.a -L. -o main -$O

shared: 
	$(CC) $(CFLAGS) -c -fPIC library.c
	$(CC) -shared library.o -o liblibrary.so
	rm -f library.o

main_shared: shared
	$(CC) $(CFLAGS) main.c -L. -Wl,-rpath=. -llibrary -o main -$O

main_dynamic: shared
	$(CC) $(CFLAGS) main.c -ldl -D DYNAMIC -o main -$O

test:
	./main $(shell cat $(TEST_ARGS_FILE))

test_O0:
	printf "\n static O0: \n" >> $(REPORT_FILE)
	make main_static O=O0
	make test
	make clean

	printf "\n shared O0:\n" >> $(REPORT_FILE)
	make main_shared O=O0
	make test
	make clean

	printf "\n dynamic O0:\n" >> $(REPORT_FILE)
	make main_dynamic O=O0
	make test
	make clean

test_O:
	printf "\n static O:\n" >> $(REPORT_FILE)
	make main_static O=O
	make test
	make clean

	printf "\n shared O:\n" >> $(REPORT_FILE)
	make main_shared O=O
	make test
	make clean

	printf "\n dynamic O:\n" >> $(REPORT_FILE)
	make main_dynamic O=O
	make test
	make clean

test_O1:
	printf "\n static O1:\n" >> $(REPORT_FILE)
	make main_static O=O1
	make test
	make clean

	printf "\n shared O1:\n" >> $(REPORT_FILE)
	make main_shared O=O1
	make test
	make clean

	printf "\n dynamic O1:\n" >> $(REPORT_FILE)
	make main_dynamic O=O1
	make test
	make clean

test_O2:
	printf "\n static O2:\n" >> $(REPORT_FILE)
	make main_static O=O2
	make test
	make clean

	printf "\n shared O2:\n" >> $(REPORT_FILE)
	make main_shared O=O2
	make test
	make clean

	printf "\n dynamic O2:\n" >> $(REPORT_FILE)
	make main_dynamic O=O2
	make test
	make clean

test_O3:
	printf "\n static O3:\n" >> $(REPORT_FILE)
	make main_static O=O3
	make test
	make clean

	printf "\n shared O3:\n" >> $(REPORT_FILE)
	make main_shared O=O3
	make test
	make clean

	printf "\n dynamic O3:\n" >> $(REPORT_FILE)
	make main_dynamic O=O3
	make test
	make clean

test_Os:
	printf "\n static Os:\n" >> $(REPORT_FILE)
	make main_static O=Os
	make test
	make clean

	printf "\n shared Os:\n" >> $(REPORT_FILE)
	make main_shared O=Os
	make test
	make clean

	printf "\n dynamic Os:\n" >> $(REPORT_FILE)
	make main_dynamic O=Os
	make test
	make clean

test_all:
	rm -f $(REPORT_FILE)
	make test_O0
	make test_O
	make test_O1
	make test_O2
	make test_O3
	make test_Os


clean:
	rm -f *.o *.a *.so trash.txt temp.txt
