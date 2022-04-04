#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <unistd.h>


int main(int argc, char **argv) {
    if(argc != 2){
        fprintf( stderr, "This program should take 1 argument!!! \n");
        return 1;
    }
    int n = atoi(argv[1]);

    pid_t child_pid;
    for(int i = 0; i<n; i++) {
        child_pid = fork();
        if (child_pid == 0) {
            printf("This text comes from child process with PID: %d\n", (int) getpid());
            return 0;
        }
    }
    return 0;
}
