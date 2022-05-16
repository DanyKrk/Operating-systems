#include "header.h"

int oven_shm_fd, table_shm_fd;
sem_t *sem_addr[SEM_NUM];
char sem_names[SEM_NUM][NAME_SIZE];
Oven *oven;
Table *table;



int main(){
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    oven_shm_fd = get_oven_shm_fd();
    table_shm_fd = get_table_shm_fd();
    fill_sem_names(sem_names);
    fill_sem_addr(sem_addr, sem_names);


    oven = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, oven_shm_fd, 0);
    table = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, table_shm_fd, 0);

    srand(getpid());
    while(1) {
        int my_pizza_type = rand() % 10;
        printf("PID: %d: %s: Przygotowuje pizze: %d\n", getpid(), get_current_time(), my_pizza_type);

        sleep(PIZZA_PREPARATION_TIME);


        add_val_to_sem(sem_addr, FULL_OVEN_SEM_ID, -1);
        add_val_to_sem(sem_addr, OVEN_ACCESS_SEM_ID, -1);
        int my_pizza_id = oven -> next_in_id;
        oven -> next_in_id = (oven -> next_in_id + 1) % OVEN_SIZE;
        oven -> pizzas[my_pizza_id] = my_pizza_type;
        oven -> pizzas_num += 1;

        printf("PID: %d: %s: Dodałem pizzę: %d. Liczba pizz w piecu: %d\n", getpid(), get_current_time(), my_pizza_type,
               oven -> pizzas_num);

        add_val_to_sem(sem_addr, OVEN_ACCESS_SEM_ID, 1);

        sleep(PIZZA_BAKING_TIME);

        add_val_to_sem(sem_addr, OVEN_ACCESS_SEM_ID, -1);

        oven -> pizzas_num -= 1;
        int oven_pizzas_num = oven -> pizzas_num;

        add_val_to_sem(sem_addr, FULL_OVEN_SEM_ID, 1);
        add_val_to_sem(sem_addr, OVEN_ACCESS_SEM_ID, 1);

        add_val_to_sem(sem_addr, FULL_TABLE_SEM_ID, -1);
        add_val_to_sem(sem_addr, TABLE_ACCESS_SEM_ID, -1);

        my_pizza_id = table -> next_in_id;
        table -> next_in_id = (table -> next_in_id + 1) % TABLE_SIZE;
        table -> pizzas[my_pizza_id] = my_pizza_type;
        table -> pizzas_num += 1;
        int table_pizzas_num = table -> pizzas_num;


        printf("PID: %d: %s: Wyjąłem pizzę: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(),
               get_current_time(), my_pizza_type, oven_pizzas_num, table_pizzas_num);

        add_val_to_sem(sem_addr, EMPTY_TABLE_SEM_ID, 1);
        add_val_to_sem(sem_addr, TABLE_ACCESS_SEM_ID, 1);
    }
}
