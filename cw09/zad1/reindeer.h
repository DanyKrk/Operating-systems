#ifndef OPERATING_SYSTEMS_RAINDEER_H
#define OPERATING_SYSTEMS_RAINDEER_H

#include "header.h"

void reindeer_wait() {
    pthread_mutex_lock(&reindeer_wait_mutex);
    while (waiting_reindeer != 0)
        pthread_cond_wait(&reindeer_wait_cond, &reindeer_wait_mutex);
    pthread_mutex_unlock(&reindeer_wait_mutex);
}

void reindeer_deliver_presents(int id) {
    pthread_mutex_lock(&reindeer_mutex);
    waiting_reindeer++;
    printf("Renifer: czeka %d reniferów na Mikołaja, %d\n", waiting_reindeer, id);
    if (waiting_reindeer == REINDEER_NUM) {
        printf("Renifer: wybudzam Mikołaja, %d\n", id);
        pthread_mutex_lock(&santa_mutex);
        pthread_cond_broadcast(&santa_cond);
        pthread_mutex_unlock(&santa_mutex);
    }
    pthread_mutex_unlock(&reindeer_mutex);
}

void* reindeer(void* arg){
    int id = *((int *) arg);
    while(true){
        reindeer_wait();
        sleep(REINDEER_HOLIDAY_TIME);
        reindeer_deliver_presents(id);
    }
}
#endif
