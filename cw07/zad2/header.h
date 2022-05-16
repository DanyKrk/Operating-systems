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
#include "semaphore.h"
#include "sys/stat.h"
#include "fcntl.h"
#include "sys/mman.h"
#include <string.h>

#define TABLE_SIZE 5
#define OVEN_SIZE 5
#define NAME_SIZE 30

#define PIZZA_PREPARATION_TIME 1
#define TRAVEL_TIME 5
#define PIZZA_BAKING_TIME 4

#define OVEN_NAME "Oven"
#define TABLE_NAME "Table"
#define SEM_NAME "Semaphores"

#define SEM_NUM 5
#define OVEN_ACCESS_SEM_ID 0
#define TABLE_ACCESS_SEM_ID 1
#define FULL_OVEN_SEM_ID 2
#define FULL_TABLE_SEM_ID 3
#define EMPTY_TABLE_SEM_ID 4

#define EMPTY_OVEN_SEM "/EMPTY_OVEN_S"
#define OVEN_SEM "/OVEN_S"
#define FULL_OVEN_SEM "/FULL_OVEN_S"
#define EMPTY_TABLE_SEM "/EMPTY_TABLE_S"
#define TABLE_SEM "/TABLE_S"
#define FULL_TABLE_SEM "/FULL_TABLE_S"


void fill_sem_names(char sem_names[SEM_NUM][NAME_SIZE]){
    strcpy(sem_names[0], "zero sem");
    strcpy(sem_names[1], "one sem");
    strcpy(sem_names[2], "two sem");
    strcpy(sem_names[3], "three sem");
    strcpy(sem_names[4], "four sem");
}

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

int get_oven_shm_fd(){
    return shm_open(OVEN_NAME, O_RDWR | O_CREAT, 0666);
}

int get_table_shm_fd(){
    return shm_open(TABLE_NAME, O_RDWR | O_CREAT, 0666);
}

void fill_sem_addr(sem_t *sem_addr[SEM_NUM], char sem_names[SEM_NUM][NAME_SIZE]){
    for(int i = 0; i < SEM_NUM; i++){
        int value = 0;
        sem_addr[i] = sem_open(sem_names[i], O_RDWR | O_CREAT, 0666, value);
        if(sem_addr[i] == SEM_FAILED) {
            perror("Problem with creating semaphore");
            exit(1);
        }
    }
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

void print_sem_val(sem_t *sem){
    int val;
    sem_getvalue(sem, &val);
    printf("Value of semafor: %d\n", val);
}

//void add_val_to_sem(sem_t *sem_addr[SEM_NUM], int sem_id, int val){
//    printf("Sem val before adding %d: ", val);
//    print_sem_val(sem_addr[sem_id]);
//
//    if(val >= 0){
//        for(int i = 0; i < val; i++){
//            sem_post(sem_addr[sem_id]);
//        }
//    }
//    else{
//        for(int i = 0; i < -val; i++){
//            sem_wait(sem_addr[sem_id]);
//        }
//    }
//    printf("Sem val after adding %d: ", val);
//    print_sem_val(sem_addr[sem_id]);
//}

void add_val_to_sem(sem_t *sem, int val){
    if(val >= 0){
        for(int i = 0; i < val; i++){
            sem_post(sem);
        }
    }
    else{
        for(int i = 0; i < -val; i++){
            sem_wait(sem);
        }
    }
}

sem_t* get_sem(char* name){
    sem_t * sem = sem_open(name, O_RDWR);
    return sem;
}



#endif //CW07_HEADER_H
