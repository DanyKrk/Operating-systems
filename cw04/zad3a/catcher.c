#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>


#define KILL 0
#define SIGQUEUE 1
#define SIGRT 2

static int received_sig_num = 0;
static int mode;
static sigset_t pass_2_signals_mask;

void handler(int signo, siginfo_t *info, void *context) {
    if (signo == SIGUSR1 || signo == SIGRTMIN) {
        received_sig_num++;
        printf("Catcher - received signal 1 #%d\n", received_sig_num);
        sigsuspend(&pass_2_signals_mask);
    }
    else if (signo == SIGUSR2 || signo == SIGRTMIN + 1) {
        printf("Catcher - received signal 2\n");
        printf("Catcher - catcher received %d signals\n", received_sig_num);
        pid_t sender_pid = info->si_pid;
        int signal1, signal2;
        if(mode == SIGRT){
            signal1 = SIGRTMIN;
            signal2 = SIGRTMIN + 1;
        }
        else{
            signal1 = SIGUSR1;
            signal2 = SIGUSR2;
        }
        if (mode == SIGQUEUE) {
            union sigval val;
            for (int i=0; i < received_sig_num; ++i) {
                val.sival_int = i;
                sigqueue(sender_pid, signal1, val);
                printf("Catcher - sent signal 1 #%d using SIGQUEUE\n", i + 1);
            }
            val.sival_int = received_sig_num;
            sigqueue(sender_pid, signal2, val);
            printf("Catcher - sent signal 2 using SIGQUEUE\n");
        }
        else {
            for (int i=0; i < received_sig_num; ++i) {
                kill(sender_pid, signal1);
                printf("Catcher - sent signal 1 #%d using KILL\n", i + 1);
            }
            kill(sender_pid, signal2);
            printf("Catcher - sent signal 2 using KILL\n");
        }
        exit(0);
    }
}

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("Program should take 1 argument!!!");
        exit(1);
    }

    if (strcmp(argv[1], "KILL") == 0) {
        mode = KILL;
    }
    else if (strcmp(argv[1], "SIGQUEUE") == 0) {
        mode = SIGQUEUE;
    }
    else if (strcmp(argv[1], "SIGRT") == 0) {
        mode = SIGRT;
    }
    else {
        perror("Mode should be KILL, SIGQUEUE or SIGRT!!!");
        exit(1);
    }

    printf("Catcher - PID: %d\n", getpid());

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

    sigsuspend(&pass_2_signals_mask);
    return 0;
}