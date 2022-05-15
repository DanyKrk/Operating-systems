//
// Created by daniel on 14.05.22.
//

#ifndef CW07_HEADER_H
#define CW07_HEADER_H

#include <pwd.h>
#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "sys/sem.h"
#include "sys/ipc.h"
#include "sys/types.h"
#include "sys/shm.h"

#define TABLE_SIZE 5
#define OVEN_SIZE 5
#define OVEN_PROJ_ID 2115
#define TABLE_PROJ_ID 2137
#define SEM_PROJ_ID 2905
#define OVEN_SEM_ID 0
#define TABLE_SEM_ID 1


typedef struct {
    int pizzas[TABLE_SIZE];
    int pizzas_num;
    int next_in_id;
    int next_out_id;
} Table;

typedef struct {
    int pizzas[OVEN_SIZE];
    int pizzas_num;
    int next_in_id;
    int next_out_id;
} Oven;

union semun {
    int              val;    /* Value for SETVAL */
    struct semid_ds *buf;    /* Buffer for IPC_STAT, IPC_SET */
    unsigned short  *array;  /* Array for GETALL, SETALL */
    struct seminfo  *__buf;  /* Buffer for IPC_INFO
                                           (Linux-specific) */
};


char* get_home_path(){
    char* path = getenv("HOME");
    if (path == NULL)
        path = getpwuid(getuid())->pw_dir;
    return path;
}

#endif //CW07_HEADER_H
