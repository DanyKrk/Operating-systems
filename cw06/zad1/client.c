#include "header.h"

int client_id;
int server_queue_id;
int client_queue_id;

void handle_delete_queue(){
    delete_queue(client_queue_id);
    exit(0);
}

void add_text_to_message(Message* msg){
    printf("Enter message:\n");
    char* msg_text_buffer = calloc(MAX_MESSAGE_TEXT_LEN, sizeof(char));
    fgets(msg_text_buffer, MAX_MESSAGE_TEXT_LEN, stdin);
    strcpy(msg->mtext, msg_text_buffer);
    free(msg_text_buffer);
}

void send_2all_msg(){
    printf("Client sending 2ALL message\n");

    time_t seconds_since_epoch = time(NULL);
    Message msg = {.client_id = client_id, .mtype = TO_ALL, .time=*localtime(&seconds_since_epoch)};

    add_text_to_message(&msg);

    send_message(server_queue_id, &msg);
}

void send_2one_msg(){
    printf("Client sending 2ONE message\n");

    time_t seconds_since_epoch = time(NULL);
    Message msg = {.client_id = client_id, .mtype = TO_ONE, .time=*localtime(&seconds_since_epoch)};

    printf("Enter target client id:\n");
    char* target_client_id_buffer = calloc(MAX_MESSAGE_TEXT_LEN, sizeof(char));
    fgets(target_client_id_buffer, MAX_MESSAGE_TEXT_LEN, stdin);
    msg.target_client_id = atoi(target_client_id_buffer);
    free(target_client_id_buffer);

    add_text_to_message(&msg);

    send_message(server_queue_id, &msg);
}


void send_list_msg(){
    printf("Client sending LIST message\n");
    time_t seconds_since_epoch = time(NULL);
    Message msg = {.mtype = LIST, .time=*localtime(&seconds_since_epoch)};
    send_message(server_queue_id, &msg);
}

void send_stop_msg(){
    printf("Client sending STOP message\n");
    time_t seconds_since_epoch = time(NULL);
    Message msg = {.client_id = client_id, .mtype = STOP, .time=*localtime(&seconds_since_epoch)};
    send_message(server_queue_id, &msg);
    handle_delete_queue();
}

void connect_with_server(){
    time_t seconds_since_epoch = time(NULL);
    Message msg = {.queue_id = client_queue_id, .mtype = INIT, .time=*localtime(&seconds_since_epoch)};
    send_message(server_queue_id, &msg);

    Message received_msg;
    receive_message(client_queue_id, &received_msg, INIT);

    client_id = received_msg.client_id;
    printf("Client id assigned by server: %d\n", received_msg.client_id);
}

int queue_has_messages(int q){
    struct msqid_ds buf;
    msgctl(q, IPC_STAT, &buf);

    if (buf.msg_qnum != 0) return 1;
    return 0;
}

void receive_messages(){
    while (queue_has_messages(client_queue_id)){
        Message msg;
        receive_message(client_queue_id, &msg, 0);

        char buff[MAX_MESSAGE_TEXT_LEN];
        switch (msg.mtype)
        {
            case STOP:
                printf("Client received STOP message\n");
                time_t seconds_since_epoch = time(NULL);
                Message message_to_server = {.mtype = STOP, .time=*localtime(&seconds_since_epoch)};
                send_message(server_queue_id, &message_to_server);
                handle_delete_queue();
                exit(0);
            case TO_ALL:
                printf("Client received 2ALL message\n");
                strftime(buff, MAX_MESSAGE_TEXT_LEN, "%Y-%m-%d %H:%M:%S", &msg.time);
                printf("Sender id: %d\nTime: %s\nText: %s\n", msg.client_id, buff, msg.mtext);
                break;
            case TO_ONE:
                printf("Client received 2ONE message\n");
                strftime(buff, MAX_MESSAGE_TEXT_LEN, "%Y-%m-%d %H:%M:%S", &msg.time);
                printf("Sender id: %d\nTime: %s\nText: %s\n", msg.client_id, buff, msg.mtext);
                break;
        }
    }
}

int stdin_has_command(){
    fd_set read_fd_set;
    struct timeval time;
    time.tv_sec = 1;
    time.tv_usec = 5;

    FD_ZERO(&read_fd_set);
    FD_SET(STDIN_FILENO, &read_fd_set);
    select(STDIN_FILENO + 1, &read_fd_set, NULL, NULL, &time);
    return (FD_ISSET(STDIN_FILENO, &read_fd_set));
}


int main(){
    printf("Client started\n");

    signal(SIGINT, send_stop_msg);
    atexit(handle_delete_queue);

    key_t server_queue_key = ftok(get_home_path(), PROJID);

    server_queue_id = msgget(server_queue_key, 0);

    pid_t pid = getpid();
    key_t client_queue_key = ftok(get_home_path(), pid);

    client_queue_id = msgget(client_queue_key, IPC_CREAT | IPC_EXCL | 0666);

    connect_with_server();
    char buffer[MAX_MESSAGE_TEXT_LEN];
    while(1){
        if (stdin_has_command()) {
            fgets(buffer, MAX_MESSAGE_TEXT_LEN, stdin);
            if (strcmp("LIST\n", buffer) == 0)
                send_list_msg();
            else if (strcmp("STOP\n", buffer) == 0)
                send_stop_msg();
            else if (strcmp("2ONE\n", buffer) == 0)
                send_2one_msg();
            else if (strcmp("2ALL\n", buffer) == 0)
                send_2all_msg();
            else
                printf("Wrong command, try again!!!\n");
        }
        receive_messages();
    }

}