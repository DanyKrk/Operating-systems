#include "header.h"

int oven_shm_id, table_shm_id, sem_id;
Oven *oven;
Table *table;

int main(){
    oven_shm_id = get_oven_shm_id();
    table_shm_id = get_table_shm_id();
    sem_id = get_sem_id();

    oven = shmat(oven_shm_id, NULL, 0);
    table = shmat(table_shm_id, NULL, 0);

    while(1) {
        struct sembuf oven_sembuf, table_sembuf;

        table_sembuf.sem_op = -1;
        table_sembuf.sem_num = TABLE_SEM_ID;
        semop(sem_id, &table_sembuf, 1);

        int table_pizzas_num = table->pizzas_num;
        while(table_pizzas_num == TABLE_SIZE){
            table_sembuf.sem_op = 1;
            semop(sem_id, &table_sembuf, 1);
            table_sembuf.sem_op = -1;
            semop(sem_id, &table_sembuf, 1);
            table_pizzas_num = table->pizzas_num;
        }

        int my_pizza_id = (table -> next_in_id - table_pizzas_num + TABLE_SIZE) % TABLE_SIZE;
        table -> next_in_id = (table -> next_in_id + 1) % TABLE_SIZE;
        table -> pizzas_num += 1;
        int my_pizza_type = table -> pizzas[my_pizza_id];

        oven_sembuf.sem_op = -1;
        oven_sembuf.sem_num = OVEN_SEM_ID;
        semop(sem_id, &oven_sembuf, 1);

        printf("%d: %s: Pobieram pizzę: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(),
               get_current_time(), my_pizza_type, oven -> pizzas_num, table -> pizzas_num);

        oven_sembuf.sem_op = 1;
        semop(sem_id, &oven_sembuf, 1);
        table_sembuf.sem_op = 1;
        semop(sem_id, &table_sembuf, 1);

        sleep(4);

        printf("%d: %s: Dostarczam pizzę: %d\n", getpid(),
               get_current_time(), my_pizza_type);

        sleep(4);
    }
}
