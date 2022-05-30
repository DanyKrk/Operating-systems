#ifndef OPERATING_SYSTEMS_SANTA_H
#define OPERATING_SYSTEMS_SANTA_H


#include "header.h"

void santa_sleep() {
    pthread_mutex_lock(&santa_mutex);
    while (waiting_elves < WORKSHOP_CAPACITY && waiting_reindeer < REINDEER_NUM)
        pthread_cond_wait(&santa_cond, &santa_mutex);
    pthread_mutex_unlock(&santa_mutex);
}

void santa_deliver_presents() {
    printf("Mikołaj: Budzę się.\n");
    deliveries++;
    printf("Mikołaj: dostarczam zabawki [licznik dostaw: %d]\n", deliveries);
    sleep(DELIVERING_TIME);

    pthread_mutex_lock(&reindeer_wait_mutex);
    waiting_reindeer = 0;
    pthread_cond_broadcast(&reindeer_wait_cond);
    pthread_mutex_unlock(&reindeer_wait_mutex);
}

void santa_solve_elf_problems() {
    printf("Mikołaj: Budzę się.\n");
    printf("Mikołaj: rozwiązuje problemy elfów [%d][%d][%d]\n", elves_queue[0], elves_queue[1], elves_queue[2]);
    for (int i = 0; i < 3; i++)
        printf("Elf: Mikołaj rozwiązuje problem, ID: %d\n", elves_queue[i]);
    sleep(SANTA_SOLVING_TIME);

    pthread_mutex_lock(&elf_wait_mutex);
    reset_queue(elves_queue);
    waiting_elves = 0;
    pthread_cond_broadcast(&elf_wait_cond);
    pthread_mutex_unlock(&elf_wait_mutex);
}

void* santa(){
    while (true){
        santa_sleep();

        pthread_mutex_lock(&reindeer_mutex);
        if (waiting_reindeer == REINDEER_NUM)
            santa_deliver_presents();
        pthread_mutex_unlock(&reindeer_mutex);

        if (deliveries == 3) break;

        pthread_mutex_lock(&elf_mutex);
        if (waiting_elves == WORKSHOP_CAPACITY)
            santa_solve_elf_problems();
        pthread_mutex_unlock(&elf_mutex);

        printf("Mikołaj: zasypiam\n");
    }
    exit(0);
}
#endif
