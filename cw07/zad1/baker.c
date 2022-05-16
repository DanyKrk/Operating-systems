#include "header.h"

int oven_shm_id, table_shm_id, sem_set_id;
Oven *oven;
Table *table;



int main(){
    oven_shm_id = get_oven_shm_id();
    table_shm_id = get_table_shm_id();
    sem_set_id = get_sem_id();

    oven = shmat(oven_shm_id, NULL, 0);
    table = shmat(table_shm_id, NULL, 0);

    struct sembuf sembuf;

    while(1) {
        srand(getpid());
        int my_pizza_type = rand() % 10;
        printf("PID: %d: %s: Przygotowuje pizze: %d\n", getpid(), get_current_time(), my_pizza_type);

        sleep(1);

        add_val_to_sem(sem_set_id, FULL_OVEN_SEM_ID, sembuf, -1);
        add_val_to_sem(sem_set_id, OVEN_ACCESS_SEM_ID, sembuf, -1);
        int my_pizza_id = oven -> next_in_id;
        oven -> next_in_id = (oven -> next_in_id + 1) % OVEN_SIZE;
        oven -> pizzas[my_pizza_id] = my_pizza_type;
        oven -> pizzas_num += 1;

        printf("PID: %d: %s: Dodałem pizzę: %d. Liczba pizz w piecu: %d\n", getpid(), get_current_time(), my_pizza_type,
               oven -> pizzas_num);

        add_val_to_sem(sem_set_id, OVEN_ACCESS_SEM_ID, sembuf, 1);

        sleep(4);

        add_val_to_sem(sem_set_id, OVEN_ACCESS_SEM_ID, sembuf, -1);

        oven -> pizzas_num -= 1;
        int oven_pizzas_num = oven -> pizzas_num;

        add_val_to_sem(sem_set_id, FULL_OVEN_SEM_ID, sembuf, 1);
        add_val_to_sem(sem_set_id, OVEN_ACCESS_SEM_ID, sembuf, 1);

        add_val_to_sem(sem_set_id, FULL_TABLE_SEM_ID, sembuf, -1);
        add_val_to_sem(sem_set_id, TABLE_ACCESS_SEM_ID, sembuf, -1);

        my_pizza_id = table -> next_in_id;
        table -> next_in_id = (table -> next_in_id + 1) % TABLE_SIZE;
        table -> pizzas[my_pizza_id] = my_pizza_type;
        table -> pizzas_num += 1;
        int table_pizzas_num = table -> pizzas_num;

        add_val_to_sem(sem_set_id, EMPTY_TABLE_SEM_ID, sembuf, 1);
        add_val_to_sem(sem_set_id, TABLE_ACCESS_SEM_ID, sembuf, 1);


        printf("PID: %d: %s: Wyjąłem pizzę: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(),
               get_current_time(), my_pizza_type, oven_pizzas_num, table_pizzas_num);
    }
}
