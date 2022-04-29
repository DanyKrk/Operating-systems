#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <errno.h>
#include <time.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include "constants.h"

int server_queue;
int clients[SERVER_CLIENT_INFO_ARRAY_SIZE];

int find_free() {
    for (int i=0; i < SERVER_CLIENT_INFO_ARRAY_SIZE; ++i) {
        if (clients[i] == 0)
            return i;
    }
    return -1;
}

void handle_init(char *content) {
    int index = find_free();
    if (index == -1) {
        ERROR(0, 0, "Error: clients limit exceeded\n");
        return;
    }
    key_t key = strtol(content, NULL, 10);
    clients[index] = msgget(key, 0);
    if (clients[index] == -1) {
        ERROR(1, 1, "Error: client %d queue could not be opened\n", index);
    }

    struct Communique feedback;
    feedback.type = INIT;
    feedback.receiver_id = index;
    if (msgsnd(clients[index], &feedback, MSGSND_COMMUNIQUE_SIZE, 0) == -1) {
        ERROR(1, 1, "Error: initial message do client %d could not be sent\n", index);
    }
}

void handle_stop(int sender_id) {
    if (clients[sender_id] != 0) {
        clients[sender_id] = 0;
    }
    else {
        ERROR(1, 1, "Error: received stop message form client that does not exist\n");
    }
}

void handle_list(int sender_id) {
    struct Communique feedback;
    feedback.type = LIST;
    feedback.content[0] = '\0';
    char buffer[20];
    for (int i=0; i < SERVER_CLIENT_INFO_ARRAY_SIZE; ++i) {
        if (clients[i] != 0) {
            sprintf(buffer, "%d ", i);
            if (strlen(feedback.content) + strlen(buffer) + 1 > MSG_SIZE) {
                ERROR(0, 0, "Error: list of clients is too long to be send\n");
                return;
            }
            strcat(feedback.content, buffer);
        }
    }

    if (msgsnd(clients[sender_id], &feedback, MSGSND_COMMUNIQUE_SIZE, 0) == -1) {
        ERROR(1, 1, "Error: list message could not be send to client %d\n", sender_id);
    }
}

void handle_toall(int sender_id, char *content) {
    struct Communique feedback;
    feedback.type = TOALL;
    feedback.sender_id = sender_id;
    strcpy(feedback.content, content);
    for (int i=0; i < SERVER_CLIENT_INFO_ARRAY_SIZE; ++i) {
        if (clients[i] != 0) {
            feedback.send_time = time(NULL);
            if (msgsnd(clients[i], &feedback, MSGSND_COMMUNIQUE_SIZE, 0) == -1) {
                ERROR(1, 1, "Error: 2all message could not be send to client %d\n", i);
            }
        }
    }
}

void handle_toone(int sender_id, int receiver_id, char *content) {
    struct Communique feedback;
    feedback.type = TOALL;
    feedback.sender_id = sender_id;
    feedback.receiver_id = receiver_id;
    strcpy(feedback.content, content);

    if (clients[feedback.receiver_id] == 0) {
        ERROR(0, 0, "Error: 2one message receiver does not exist\n");
        return;
    }
    feedback.send_time = time(NULL);
    if (msgsnd(clients[feedback.receiver_id], &feedback, MSGSND_COMMUNIQUE_SIZE, 0) == -1) {
        ERROR(1, 1, "Error: 2one to client %s message could not be send\n", feedback.receiver_id);
    }
}

void sigint_handler(int exit_val) {
    exit(exit_val);
}

void closing_procedure() {
    struct Communique close_communique;
    close_communique.type = STOP;
    for (int i=0; i < SERVER_CLIENT_INFO_ARRAY_SIZE; ++i) {
        if (clients[i] != 0) {
            if (msgsnd(clients[i], &close_communique, MSGSND_COMMUNIQUE_SIZE, 0) == -1) {
                perror("Problem with sending STOP to client!!!");
                exit(1);
            }
        }
    }

    if (msgctl(server_queue, IPC_RMID, NULL) == -1) {
        perror("Problem with sending STOP to client!!!");
        exit(1);
    }
}

int create_server_queue(key_t *key) {
    if ((*key = ftok(SERVER_PATH, PROJ_ID)) == -1) {
        perror("Problem with generating IPC key!!!");
        exit(1);
    }

    int sq_desc = msgget(*key, IPC_CREAT | 0666);
    if (sq_desc == -1) {
        ERROR(1, 1, "Error: server queue cannot be open\n");
    }

    return sq_desc;
}

int main(void) {
    if (signal(SIGINT, sigint_handler) == SIG_ERR) {
        perror("Problem with setting SIGINT procedure!!!");
        exit(1);
    }

    if (atexit(closing_procedure) == -1) {
        perror("Problem with setting closing procedure!!!");
        exit(1);
    }

    FILE *communiques_log_file = fopen("./communiques_log_file.txt", "a");

    key_t server_queue_key;
    server_queue = create_server_queue(&server_queue_key);

    for (int i=0; i < SERVER_CLIENT_INFO_ARRAY_SIZE; ++i) clients[i] = 0;

    char time_buff[20];
    struct Communique communique;
    while(1) {
        if (msgrcv(server_queue, &communique, MSGSND_COMMUNIQUE_SIZE, -6, 0) == -1) {
            ERROR(1, 1, "Error: message form server queue could not be receieved\n");
        }
        strftime(time_buff, 20, "%Y-%m-%d %H:%M:%S", localtime(&communique.send_time));
        fprintf(communiques_log_file, "TIME: %s | SENDER_ID: %d | TYPE: %d | CONTENT: %s\n", \
                time_buff, communique.sender_id, communique.type, communique.content);
        fflush(communiques_log_file);
        switch (communique.type) {
            case STOP:
                handle_stop(communique.sender_id);
                break;
            case LIST:
                handle_list(communique.sender_id);
                break;
            case TOALL:
                handle_toall(communique.sender_id, communique.content);
                break;
            case TOONE:
                handle_toone(communique.sender_id, communique.receiver_id, communique.content);
                break;
            case INIT:
                handle_init(communique.content);
                break;
            default: ERROR(0, 0, "Error: received message type does not exist\n");
                break;
        }
    }
    fclose(communiques_log_file);
    return 0;
}