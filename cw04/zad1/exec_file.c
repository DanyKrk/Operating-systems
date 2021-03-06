
#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>

int main(int argc, char **argv){
    if (strcmp(argv[1], "pending") != 0){
        raise(SIGUSR1);
        printf("after second raise\n");
    }
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
        sigset_t pending_set;
        sigpending(&pending_set);
        printf("SIGUSR1 pending - exec_file: %d\n", sigismember(&pending_set, SIGUSR1));
    }
    printf("program end\n");
}