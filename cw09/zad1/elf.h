#ifndef OPERATING_SYSTEMS_ELF_H
#define OPERATING_SYSTEMS_ELF_H

#include "header.h"

void elf_wait(int id) {
    pthread_mutex_lock(&elf_wait_mutex);
    while (waiting_elves == WORKSHOP_CAPACITY) {
        printf("Elf: czeka na powrót elfów, ID: %d\n", id);
        pthread_cond_wait(&elf_wait_cond, &elf_wait_mutex);
    }
    pthread_mutex_unlock(&elf_wait_mutex);
}

void elf_report_problem(int id) {
    pthread_mutex_lock(&elf_mutex);
    if (waiting_elves < WORKSHOP_CAPACITY) {
        elves_queue[waiting_elves] = id;
        waiting_elves++;
        printf("Elf: czeka %d elfów na Mikołaja, ID: %d\n", waiting_elves, id);

        if (waiting_elves == WORKSHOP_CAPACITY){
            printf("Elf: wybudzam Mikołaja, ID: %d\n", id);
            pthread_mutex_lock(&santa_mutex);
            pthread_cond_broadcast(&santa_cond);
            pthread_mutex_unlock(&santa_mutex);
        }
    }
    pthread_mutex_unlock(&elf_mutex);
}

void* elf(void* arg){
    int id = *((int *) arg);
    while(true){
        sleep(ELF_WORKING_TIME);
        elf_wait(id);
        elf_report_problem(id);
    }
}

#endif //OPERATING_SYSTEMS_ELF_H
