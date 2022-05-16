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

    while(1) {
        add_val_to_sem(sem_addr, EMPTY_TABLE_SEM_ID, -1);
        add_val_to_sem(sem_addr, TABLE_ACCESS_SEM_ID, -1);

        int table_pizzas_num = table->pizzas_num;

        int my_pizza_id = (table -> next_in_id - table_pizzas_num + TABLE_SIZE) % TABLE_SIZE;
        table -> next_in_id = (table -> next_in_id + 1) % TABLE_SIZE;
        table -> pizzas_num -= 1;
        int my_pizza_type = table -> pizzas[my_pizza_id];

        add_val_to_sem(sem_addr, FULL_TABLE_SEM_ID, 1);
        add_val_to_sem(sem_addr, OVEN_ACCESS_SEM_ID, -1);

        printf("PID: %d: %s: Pobieram pizzę: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(),
               get_current_time(), my_pizza_type, oven -> pizzas_num, table -> pizzas_num);


        add_val_to_sem(sem_addr, TABLE_ACCESS_SEM_ID, 1);
        add_val_to_sem(sem_addr, OVEN_ACCESS_SEM_ID, 1);

        sleep(TRAVEL_TIME);

        printf("PID: %d: %s: Dostarczam pizzę: %d\n", getpid(),
               get_current_time(), my_pizza_type);

        sleep(TRAVEL_TIME);
    }
}
