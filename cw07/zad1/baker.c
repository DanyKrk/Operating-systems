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
        int my_pizza_type = rand() % 10;
        printf("%d: %s: Przygotowuje pizze: %d\n", getpid(), get_current_time(), my_pizza_type);

        sleep(1);

        struct sembuf oven_sembuf, table_sembuf;
        oven_sembuf.sem_num = OVEN_SEM_ID;
        oven_sembuf.sem_op = -1;
        semop(sem_id, &oven_sembuf, 1);
        int oven_pizzas_num = oven->pizzas_num;
        while(oven_pizzas_num == OVEN_SIZE){
            oven_sembuf.sem_op = 1;
            semop(sem_id, &oven_sembuf, 1);
            oven_sembuf.sem_op = -1;
            semop(sem_id, &oven_sembuf, 1);
            oven_pizzas_num = oven->pizzas_num;
        }
        int my_pizza_id = oven -> next_in_id;
        oven -> next_in_id = (oven -> next_in_id + 1) % OVEN_SIZE;
        oven -> pizzas[my_pizza_id] = my_pizza_type;
        oven -> pizzas_num += 1;

        printf("%d: %s: Dodałem pizzę: %d. Liczba pizz w piecu: %d\n", getpid(), get_current_time(), my_pizza_type,
               oven -> pizzas_num);

        oven_sembuf.sem_op = 1;
        semop(sem_id, &oven_sembuf, 1);

        sleep(4);

        oven_sembuf.sem_op = -1;
        semop(sem_id, &oven_sembuf, 1);
        oven -> pizzas_num -= 1;
        oven_sembuf.sem_op = 1;
        semop(sem_id, &oven_sembuf, 1);
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

        my_pizza_id = table -> next_in_id;
        table -> next_in_id = (oven -> next_in_id + 1) % OVEN_SIZE;
        table -> pizzas[my_pizza_id] = my_pizza_type;
        table -> pizzas_num += 1;

        oven_sembuf.sem_op = -1;
        semop(sem_id, &oven_sembuf, 1);

        printf("%d: %s: Wyjąłem pizzę: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(),
               get_current_time(), my_pizza_type, oven -> pizzas_num, table -> pizzas_num);

        oven_sembuf.sem_op = 1;
        semop(sem_id, &oven_sembuf, 1);
        table_sembuf.sem_op = 1;
        semop(sem_id, &table_sembuf, 1);
    }
}
