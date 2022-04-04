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
    printf("program start\n");
    if (strcmp(argv[1], "ignore") == 0){
//        signal(SIGUSR1, SIG_IGN);
        struct sigaction act;
        act.sa_handler = SIG_IGN;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR1, &act, NULL);
    }

    else if (strcmp(argv[1], "handler") == 0){
//        signal(SIGUSR1, handler);
        struct sigaction act;
        act.sa_handler = handler;
        sigemptyset(&act.sa_mask);
        act.sa_flags = 0;
        sigaction(SIGUSR1, &act, NULL);
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
    printf("after first raise - before exec\n");
    execl("./exec_file", "./exec_file", argv[1], NULL);
}