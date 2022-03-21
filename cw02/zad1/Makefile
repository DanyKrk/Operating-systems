CC = gcc
CFLAGS = -Wall
REPORT_FILE = raport2.txt
TEST_ARGS_FILE = test_args.txt

test:
	make main
	./main src_file target_file

main:
	$(CC) $(CFLAGS) main.c lib_copy.c sys_copy.c -o main

clean:
	rm -f *.o *.a *.so 
