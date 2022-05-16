//
// Created by daniel on 14.05.22.
//

#ifndef CW07_HEADER_H
#define CW07_HEADER_H

#include <pwd.h>
#include <unistd.h>
#include <sys/time.h>
#include <time.h>
#include "stdio.h"
#include "stdlib.h"
#include "sys/sem.h"
#include "sys/ipc.h"
#include "sys/types.h"
#include "sys/shm.h"
#include "signal.h"
#include "sys/wait.h"
#include <sys/prctl.h>

#define TABLE_SIZE 5
#define OVEN_SIZE 5

#define PIZZA_PREPARATION_TIME 1
#define TRAVEL_TIME 5
#define PIZZA_BAKING_TIME 4

#define OVEN_PROJ_ID 2115
#define TABLE_PROJ_ID 2137
#define SEM_PROJ_ID 2905

#define OVEN_ACCESS_SEM_ID 0
#define TABLE_ACCESS_SEM_ID 1
#define FULL_OVEN_SEM_ID 2
#define FULL_TABLE_SEM_ID 3
#define EMPTY_TABLE_SEM_ID 4





typedef struct {
    int pizzas[TABLE_SIZE];
    int pizzas_num;
    int next_in_id;
} Table;

typedef struct {
    int pizzas[OVEN_SIZE];
    int pizzas_num;
    int next_in_id;
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

int get_oven_shm_id(){
    key_t oven_key;
    char *home_path = get_home_path();
    oven_key = ftok(home_path, OVEN_PROJ_ID);
    return shmget(oven_key, sizeof(Oven), 0);
}

int get_table_shm_id(){
    key_t table_key;
    char *home_path = get_home_path();
    table_key = ftok(home_path, TABLE_PROJ_ID);
    return shmget(table_key, sizeof(Table), 0);
}

int get_sem_id(){
    key_t sem_key;
    char *home_path = get_home_path();
    sem_key = ftok(home_path, SEM_PROJ_ID);
    return semget(sem_key, 5, 0);
}

char* get_current_time(){
    struct timeval cur_time;
    gettimeofday(&cur_time, NULL);
    int ml_sec = cur_time.tv_usec / 1000;

    char* buffer = calloc(50, sizeof(char));
    strftime(buffer, 50, "%H:%M:%S", localtime(&cur_time.tv_sec));

    char* current_time = calloc(50, sizeof(char));
    sprintf(current_time, "%s:%03d", buffer, ml_sec);
    return current_time;
}

void add_val_to_sem(int sem_set_id, int sem_id, struct sembuf sembuf, int val){
    sembuf.sem_num = sem_id;
    sembuf.sem_op = val;
    semop(sem_set_id, &sembuf, 1);
}

#endif //CW07_HEADER_H
