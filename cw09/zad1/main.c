#include "santa.h"
#include "elf.h"
#include "reindeer.h"

int main(){
    reset_queue(elves_queue);

    pthread_t santa_t;
    pthread_create(&santa_t, NULL, &santa, NULL);
    pthread_t* reindeer_t = initialize_threads(REINDEER_NUM, reindeer);
    pthread_t* elves_t = initialize_threads(ELVES_NUM, elf);

    join_threads(santa_t, reindeer_t, elves_t);
    return 0;
}