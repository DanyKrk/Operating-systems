#ifndef CW06_CONSTANTS_H
#define CW06_CONSTANTS_H


#define MSG_SIZE 1024  // maximum message size
#define SERVER_PATH "/"
#define PROJ_ID 1337   // project id used in key generating
#define SERVER_CLIENT_INFO_ARRAY_SIZE 20

#define STOP  1
#define LIST  2
#define TOALL 3
#define TOONE 4
#define INIT  5

#define ERROR(code, if_errno, format, ...) {                     \
    fprintf(stderr, format, ##__VA_ARGS__);                      \
    if (if_errno)                                                \
        fprintf(stderr, "Error message: %s\n", strerror(errno)); \
    if (code != 0)                                               \
        exit(code);                                              \
}                                                                \

struct Communique {
    long type;
    int sender_id;
    int receiver_id;
    time_t send_time;
    char content[MSG_SIZE];
};

#define MSGSND_COMMUNIQUE_SIZE sizeof(struct Communique) - sizeof(long int)

#endif //CW06_CONSTANTS_H
