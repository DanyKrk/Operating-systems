#define _POSIX_C_SOURCE 1
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <signal.h>
#include <string.h>
#include <sys/types.h>

void async_signal_safe_print (const char *s){
    write(STDOUT_FILENO, s, strlen(s));
}

void handler(int signal){
    char message [] = "write: I got signal!!! (handler)\n";
    async_signal_safe_print(message);
}

int main(int argc, char **argv){
    struct sigaction act;
    if (strcmp(argv[1], "ignore") == 0){
        act.sa_handler = SIG_IGN;
        if (sigaction(SIGUSR1, &act, NULL) == -1) {
            perror("sigaction failed");
            exit(EXIT_FAILURE);
        }
    }

    else if (strcmp(argv[1], "handler") == 0){
        act.sa_handler = handler;
        if (sigaction(SIGUSR1, &act, NULL) == -1) {
            perror("sigaction failed");
            exit(EXIT_FAILURE);
        }
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


    if (strcmp(argv[1], "ignore") == 0){
        act.sa_handler = SIG_IGN;
        if (sigaction(SIGUSR1, &act, NULL) == -1) {
            perror("sigaction failed");
            exit(EXIT_FAILURE);
        }
    }

    else if (strcmp(argv[1], "handler") == 0){
        act.sa_handler = handler;
        if (sigaction(SIGUSR1, &act, NULL) == -1) {
            perror("sigaction failed");
            exit(EXIT_FAILURE);
        }
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


    sigset_t pending_set;
    if (strcmp(argv[1], "mask") == 0 || strcmp(argv[1], "pending") == 0){
        sigpending(&pending_set);
        printf("SIGUSR1 pending - parent: %d\n", sigismember(&pending_set, SIGUSR1));
    }
    execl("./exec_file", "./exec_file", argv[1], NULL);
    async_signal_safe_print("program end\n");
}