
#include <stdlib.h>
#include <pwd.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/msg.h>
#include <stdio.h>
#include <signal.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#define PROJID 2115
#define MAX_CLIENTS 5
#define MAX_MESSAGE_TEXT_LEN 512

#define STOP 1
#define LIST 2
#define INIT 3
#define TO_ONE 4
#define TO_ALL 5

typedef struct {
    long mtype;
    int client_id;
    int target_client_id;
    char mtext[MAX_MESSAGE_TEXT_LEN];
    int queue_id;
    struct tm time;
} Message;

typedef struct {
    int id;
    int queue_id;
    int is_running;
} Client;

char* get_home_path(){
    char* path = getenv("HOME");
    if (path == NULL)
        path = getpwuid(getuid())->pw_dir;
    return path;
}

void send_message(int queue_id, Message* msg){
    msgsnd(queue_id, msg, sizeof(Message) - sizeof(msg->mtype), 0);
}

void receive_message(int queue_id, Message* msg, long type_to_rcv){
    msgrcv(queue_id, msg, sizeof(Message) - sizeof(msg->mtype), type_to_rcv, 0);
}

void delete_queue(int queue_id) {
    msgctl(queue_id, IPC_RMID, NULL);
}