CC = gcc
CFLAGS = -Wall

test:
	make remove_executables
	make main_exec
	make main_fork
	./main_fork ignore
	./main_fork handler
	./main_fork mask
	./main_fork pending
	./main_exec ignore
	./main_exec handler
	./main_exec mask
	./main_exec pending

exec_file:
	$(CC) $(CFLAGS) exec_file.c -o exec_file

main_exec:
	make exec_file
	$(CC) $(CFLAGS) main_exec.c -o main_exec

main_fork:
	$(CC) $(CFLAGS) main_fork.c -o main_fork

clean:
	rm -f *.o *.a *.so 

remove_executables:
	rm exec_file main_exec main_fork
