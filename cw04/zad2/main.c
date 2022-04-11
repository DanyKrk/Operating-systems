#include <stdio.h>
#include <signal.h>
#include <time.h>
#include <unistd.h>
#include <errno.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>

void handler(int sig, siginfo_t *info, void *ucontext) {
    printf("Signal number: %d\n", (int)info->si_signo);
    printf("Sending process PID: %d\n", (int)info->si_pid);
    printf("An errno value: %d\n", (int)info->si_errno);
    printf("Real user ID of sending process: %d\n", (int)info->si_uid);
    printf("System time consumed: %lf\n\n", (double)info->si_stime/sysconf(_SC_CLK_TCK));
}


int main(void) {
    printf("\n\nTesting SIGINFO flag\n\n");
    struct sigaction sigaction_SIGINFO;
    sigaction_SIGINFO.sa_flags = SA_SIGINFO;
    sigaction_SIGINFO.sa_sigaction = handler;
    sigemptyset(&sigaction_SIGINFO.sa_mask);

    if (sigaction(SIGUSR1, &sigaction_SIGINFO, NULL) == -1) {
        perror("Couldn' t set handler!!!\n");
        exit(1);
    }
    if (sigaction(SIGUSR2, &sigaction_SIGINFO, NULL) == -1) {
        perror("Couldn' t set handler!!!\n");
        exit(1);
    }
    if (sigaction(SIGRTMIN, &sigaction_SIGINFO, NULL) == -1) {
        perror("Couldn' t set handler!!!\n");
        exit(1);
    }
    raise(SIGUSR1);
    raise(SIGUSR2);
    raise(SIGRTMIN);




    printf("\n\nTesting NOCLDSTOP flag\n\n");
    struct sigaction sigaction_NOCLDSTOP;
    sigaction_NOCLDSTOP.sa_flags = SA_SIGINFO | SA_NOCLDSTOP;
    sigaction_NOCLDSTOP.sa_sigaction = handler;
    sigemptyset(&sigaction_NOCLDSTOP.sa_mask);

    if (sigaction(SIGCHLD, &sigaction_NOCLDSTOP, NULL) == -1) {
        perror("Couldn' t set handler!!!\n");
        exit(1);
    }

    pid_t child_pid;
    if ((child_pid = fork()) == -1) {
        perror("Couldn' t create child_pid process!!!\n");
        exit(1);
    }
    else if (child_pid == 0) {
        raise(SIGSTOP);
    }




    printf("\n\nTesting RESETHAND flag\n\n");
    struct sigaction sigaction_RESETHAND;
    sigaction_RESETHAND.sa_flags = SA_SIGINFO | SA_RESETHAND;
    sigaction_RESETHAND.sa_sigaction = handler;
    sigemptyset(&sigaction_RESETHAND.sa_mask);

    if (sigaction(SIGUSR1, &sigaction_RESETHAND, NULL) == -1) {
        perror("Couldn' t set handler!!!\n");
        exit(1);
    }

    raise(SIGUSR1);
    raise(SIGUSR1);
}