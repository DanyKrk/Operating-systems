#include "header.h"

FILE* server_log_file;
Client clients[MAX_CLIENTS];
int server_queue_id;

int get_client_queue_id(int client_id) {
    for(int i=0; i<MAX_CLIENTS; i++){
        if(clients[i].is_running && clients[i].id == client_id) {
            return clients[i].queue_id;
        }
    }
    return -1;
}

void add_server_log(Message* msg){
    char time_buffer[MAX_MESSAGE_TEXT_LEN];
    strftime(time_buffer, MAX_MESSAGE_TEXT_LEN, "Message arrival time: %Y-%m-%d %H:%M:%S", &msg->time);

    char log_msg_buffer[2 * MAX_MESSAGE_TEXT_LEN];
    sprintf(log_msg_buffer, "%s, sender id: %d, message type: %ld\n", time_buffer, msg->client_id, msg->mtype);

    fputs(log_msg_buffer, server_log_file);
}

void handle_list_msg(Message* msg){
    add_server_log(msg);
    printf("Server handling LIST\n");
    printf("Active clients: \n");
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running)
            printf("\tClient id: %d\n", clients[i].id);
    }
}

void handle_2one_msg(Message* msg){
    add_server_log(msg);
    printf("Server handling 2ONE\n");

    int target_client_id = msg->target_client_id;
    int target_client_queue_id = get_client_queue_id(target_client_id);
    if(target_client_queue_id == -1){
        printf("No running client with such id!!!\n");
        return;
    }
    Message msg_to_send = {.mtype = TO_ONE, .time=msg->time, .client_id = msg->client_id};
    strcpy(msg_to_send.mtext, msg->mtext);
    send_message(target_client_queue_id, &msg_to_send);
}

void handle_2all_msg(Message* msg){
    add_server_log(msg);
    printf("Server handling 2ALL\n");

    for(int i=0; i<MAX_CLIENTS; i++){
        if(clients[i].is_running) {
            Message msg_to_send = {.mtype = TO_ALL, .time=msg->time, .client_id = msg->client_id};
            strcpy(msg_to_send.mtext, msg->mtext);
            send_message(clients[i].queue_id, &msg_to_send);
        }
    }
}

void handle_init_msg(Message* msg){
    add_server_log(msg);
    printf("Server handling INIT\n");

    int client_id = -1;
    for(int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running == 0){
            client_id = i;
            printf("Assigned client ID: %d\n", i);
            break;
        }
    }

    if (client_id == -1){
        printf("Exceeded max number of clients!!!");
        exit(1);
    }

    int client_queue_id = msg->queue_id;
    Client new_client = {client_id, client_queue_id, 1};
    clients[client_id] = new_client;

    Message msg_back = {.mtype = INIT, .client_id = client_id};
    send_message(client_queue_id, &msg_back);
}

void handle_stop_msg(Message* msg){
    add_server_log(msg);
    printf("Server handling STOP\n");

    int client_id = msg->client_id;
    clients[client_id].is_running = 0;
}

void handle_msg(Message* msg){
    switch (msg->mtype) {
        case INIT:
            handle_init_msg(msg);
            break;
        case LIST:
            handle_list_msg(msg);
            break;
        case TO_ALL:
            handle_2all_msg(msg);
            break;
        case TO_ONE:
            handle_2one_msg(msg);
            break;
        case STOP:
            handle_stop_msg(msg);
            break;
    }
}

void server_exit_procedure(){
    printf("Server exit procedure\n");
    Message msg;
    int client_queue;
    msg.mtype = STOP;
    for (int i = 0; i < MAX_CLIENTS; i++){
        if (clients[i].is_running) {
            printf("\tSending STOP to client with id: %d\n", i);

            client_queue = clients[i].queue_id;
            send_message(client_queue, &msg);

            receive_message(server_queue_id, &msg, STOP);
            printf("INFO: STOP received by client\n");
        }
    }

    delete_queue(server_queue_id);
    fclose(server_log_file);
}

void handle_sigint(){
    printf("Exiting because of SIGINT\n");
    exit(0);
}

int main(){
    printf("Server started\n");

    signal(SIGINT, handle_sigint);
    atexit(server_exit_procedure);

    key_t server_queue_key = ftok(get_home_path(), PROJID);

    server_queue_id = msgget(server_queue_key, IPC_CREAT | IPC_EXCL | 0666);
    if (server_queue_id < 0){
        perror("msgget");
        return -1;
    }
    printf("Created server queue with id: %d\n", server_queue_id);


    for(int i = 0; i < MAX_CLIENTS; i++)
        clients[i].is_running = 0;

    server_log_file = fopen("server_log.txt", "w");

    Message msg;
    while(1){
        if (msgrcv(server_queue_id, &msg, sizeof(Message) - sizeof(msg.mtype), -TO_ALL, 0) != -1){
            handle_msg(&msg);
        }
    }
}