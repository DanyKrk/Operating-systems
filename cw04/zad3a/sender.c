#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <sys/types.h>


#define KILL 0
#define SIGQUEUE 1
#define SIGRT 2

static int initial_sig_num = 0;
static int received_sig_num = 0;
static sigset_t pass_2_signals_mask;


void send_signals(pid_t catcher_pid, int mode, int signal1, int signal2) {
    if (mode == SIGQUEUE) {
        union sigval val;
        for (int i=0; i < initial_sig_num; ++i) {
            sigqueue(catcher_pid, signal1, val);
            printf("Sender - sent signal 1 #%d using SIGQUEUE\n", i + 1);
        }
        sigqueue(catcher_pid, signal2, val);
        printf("Sender - sent signal 2 using SIGQUEUE\n");
        return;
    }
    else{
        for (int i=0; i < initial_sig_num; ++i) {
            kill(catcher_pid, signal1);
            printf("Sender - sent signal 1 #%d using KILL\n", i + 1);
        }
        kill(catcher_pid, signal2);
        printf("Sender - sent signal 2 using KILL\n");
    }
}

void handler(int sig, siginfo_t *info, void *ucontext) {
    if (sig == SIGUSR1 || sig == SIGRTMIN) {
        received_sig_num++;
        printf("Sender - received signal 1 #%d", received_sig_num);
        if (info->si_code == SI_QUEUE) {
            printf(" with info value: %d", info->si_value.sival_int);
        }
        printf("\n");
        sigsuspend(&pass_2_signals_mask);
    }
    else if (sig == SIGUSR2 || sig == SIGRTMIN + 1) {
        printf("Sender - received signal 2\n");
        printf("Sender - expected %d signals, and received %d signals\n", initial_sig_num, received_sig_num);
        if (info->si_code == SI_QUEUE) {
            printf("Sender - catcher received %d signals\n", info->si_value.sival_int);
        }
        exit(0);
    }
}

int main(int argc, char **argv) {
    if (argc != 4) {
        perror("Program should take 3 arguments!!!");
        exit(1);
    }

    pid_t catcher_pid = strtol(argv[1], NULL, 10);
    if (catcher_pid <= 0) {
        perror("First argument is supposed to be a catcher PID!!!");
        exit(1);
    }

    initial_sig_num = strtol(argv[2], NULL, 10);
    if (initial_sig_num <= 0) {
        perror("Number of signals to be sent should be positive!!!");
        exit(1);
    }

    int mode;
    if (strcmp(argv[3], "KILL") == 0) {
        mode = KILL;
    }
    else if (strcmp(argv[3], "SIGQUEUE") == 0) {
        mode = SIGQUEUE;
    }
    else if (strcmp(argv[3], "SIGRT") == 0) {
        mode = SIGRT;
    }
    else {
        perror("Mode should be KILL, SIGQUEUE or SIGRT!!!");
        exit(1);
    }

    int signal1, signal2;
    if(mode == SIGRT){
        signal1 = SIGRTMIN;
        signal2 = SIGRTMIN + 1;
    }
    else{
        signal1 = SIGUSR1;
        signal2 = SIGUSR2;
    }


    sigfillset(&pass_2_signals_mask);
    sigdelset(&pass_2_signals_mask, signal1);
    sigdelset(&pass_2_signals_mask, signal2);
    sigprocmask(SIG_SETMASK, &pass_2_signals_mask, NULL);

    sigset_t block_all_mask;
    sigfillset(&block_all_mask);
    struct sigaction singnals_action;
    singnals_action.sa_flags = SA_SIGINFO;
    singnals_action.sa_mask = block_all_mask;
    singnals_action.sa_sigaction = handler;
    if (sigaction(signal1, &singnals_action, NULL) == -1) {
        perror("Couldn't set signal 1 handler!!!");
        exit(1);
    }
    if (sigaction(signal2, &singnals_action, NULL) == -1) {
        perror("Couldn't set signal 2 handler!!!");
        exit(1);
    }

    send_signals(catcher_pid, mode, signal1, signal2);

    sigsuspend(&pass_2_signals_mask);
}