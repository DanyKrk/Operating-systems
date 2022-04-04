#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

void handler(int signal){
    printf("I got signal!!! (handler)\n");
}

int main(int argc, char **argv){
    if (strcmp(argv[1], "ignore") == 0){
        signal(SIGUSR1, SIG_IGN);
    }

    else if (strcmp(argv[1], "handler") == 0){
        signal(SIGUSR1, handler);
    }

    else if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
        sigset_t mask;
        sigemptyset(&mask);
        sigaddset(&mask, SIGUSR1);
        if (sigprocmask(SIG_SETMASK, &mask, NULL) < 0) {
            perror("Couldn' t set process mask!!!\n");
            exit(1);
        }
    }
    raise(SIGUSR1);
    sigset_t pending_set;
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
        sigpending(&pending_set);
        printf("SIGUSR1 pending - parent: %d\n", sigismember(&pending_set, SIGUSR1));
    }


    pid_t child_pid = fork();
    if (child_pid == 0){
        if (strcmp(argv[1], "pending") != 0){
            raise(SIGUSR1);
        }
        if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
            sigpending(&pending_set);
            printf("SIGUSR1 pending - child: %d\n", sigismember(&pending_set, SIGUSR1));
        }
    }
    printf("program end\n");
}

