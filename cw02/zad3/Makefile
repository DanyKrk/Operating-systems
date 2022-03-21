CC = gcc
CFLAGS = -Wall
REPORT_FILE = raport2.txt
TEST_ARGS_FILE = test_args.txt

test:
	make main_nftw
	make main_stat
	./main_nftw ..
	./main_stat ..

explore_stat:
	$(CC) $(CFLAGS) -c explore_stat.c -o explore_stat.o

explore_nftw:
	$(CC) $(CFLAGS) -c explore_nftw.c -o explore_nftw.o

main_nftw: explore_nftw
		$(CC) $(CFLAGS) main_nftw.c explore_nftw.o -o main_nftw

main_stat: explore_stat
		$(CC) $(CFLAGS) main_stat.c explore_stat.o -o main_stat

clean:
	rm -f *.o *.a *.so 
