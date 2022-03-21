CC = gcc
CFLAGS = -Wall
REPORT_FILE = raport2.txt
TEST_ARGS_FILE = test_args.txt

test: 
	make main
	./main a b_f

main: lib_count sys_count
	$(CC) $(CFLAGS) main.c lib_count.o sys_count.o -o main

lib_count:
	$(CC) $(CFLAGS) -c lib_count.c

sys_count:
	$(CC) $(CFLAGS) -c sys_count.c

clean:
	rm -f *.o *.a *.so 
