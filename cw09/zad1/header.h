#ifndef OPERATING_SYSTEMS_HEADER_H
#define OPERATING_SYSTEMS_HEADER_H

#include "pthread.h"
#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <stdbool.h>

#define REINDEER_NUM 9
#define ELVES_NUM 10
#define WORKSHOP_CAPACITY 3

#define ELF_WORKING_TIME rand() % 3 + 2
#define SANTA_SOLVING_TIME rand() % 1 + 1
#define REINDEER_HOLIDAY_TIME rand() % 8 + 2
#define DELIVERING_TIME rand() % 2 + 2

pthread_mutex_t santa_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t santa_cond =PTHREAD_COND_INITIALIZER;
pthread_mutex_t reindeer_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t reindeer_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t reindeer_wait_cond   = PTHREAD_COND_INITIALIZER;
pthread_mutex_t elf_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_mutex_t elf_wait_mutex = PTHREAD_MUTEX_INITIALIZER;
pthread_cond_t elf_wait_cond =PTHREAD_COND_INITIALIZER;

int waiting_reindeer = 0;
int deliveries = 0;
int waiting_elves = 0;
int elves_queue[WORKSHOP_CAPACITY];

void reset_queue(int* queue){
    for (int i = 0; i < WORKSHOP_CAPACITY; i++) queue[i] = -1;
}

pthread_t* initialize_threads(int amount, void *routine) {
    pthread_t* threads = calloc(amount, sizeof(pthread_t));
    for (int i = 0; i < amount; i++)
        pthread_create(&threads[i], NULL, routine, &i);
    return threads;
}

void join_threads(pthread_t santa_t, pthread_t* reindeer_t, pthread_t* elves_t) {
    pthread_join(santa_t, NULL);
    for (int i = 0; i < REINDEER_NUM; i++) pthread_join(reindeer_t[i], NULL);
    for (int i = 0; i < ELVES_NUM; i++) pthread_join(elves_t[i], NULL);
}

#endif //OPERATING_SYSTEMS_HEADER_H
