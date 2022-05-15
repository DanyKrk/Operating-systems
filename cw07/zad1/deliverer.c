#include "header.h"

int oven_shm_id, table_shm_id, sem_id;
Oven *oven;
Table *table;

int main(){
    oven_shm_id = get_oven_shm_id();
    table_shm_id = get_table_shm_id();
    sem_id = get_sem_id();

    oven = shmat(oven_shm_id, NULL)

}
