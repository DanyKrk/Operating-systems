#include "header.h"

int oven_shm_fd, table_shm_fd;
//sem_t *sem_addr[SEM_NUM];
//char sem_names[SEM_NUM][NAME_SIZE];
Oven *oven;
Table *table;

int main(){
    prctl(PR_SET_PDEATHSIG, SIGHUP);

    oven_shm_fd = get_oven_shm_fd();
    table_shm_fd = get_table_shm_fd();
//    fill_sem_names(sem_names);
//    fill_sem_addr(sem_addr, sem_names);
    sem_t* oven_access_sem = get_sem(OVEN_SEM);
    sem_t* full_oven_sem = get_sem(FULL_OVEN_SEM);
    sem_t* empty_table_sem = get_sem(EMPTY_TABLE_SEM);
    sem_t* table_access_sem = get_sem(TABLE_SEM);
    sem_t* full_table_sem = get_sem(FULL_TABLE_SEM);

    oven = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, oven_shm_fd, 0);
    table = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, table_shm_fd, 0);

    while(1) {
        add_val_to_sem(empty_table_sem, -1);
        add_val_to_sem(table_access_sem, -1);

        int table_pizzas_num = table->pizzas_num;

        int my_pizza_id = (table -> next_in_id - table_pizzas_num + TABLE_SIZE) % TABLE_SIZE;
        table -> next_in_id = (table -> next_in_id + 1) % TABLE_SIZE;
        table -> pizzas_num -= 1;
        int my_pizza_type = table -> pizzas[my_pizza_id];

        add_val_to_sem(full_table_sem, 1);
        add_val_to_sem(oven_access_sem, -1);

        printf("PID: %d: %s: Pobieram pizzę: %d. Liczba pizz w piecu: %d. Liczba pizz na stole: %d\n", getpid(),
               get_current_time(), my_pizza_type, oven -> pizzas_num, table -> pizzas_num);

        add_val_to_sem(table_access_sem, 1);
        add_val_to_sem(oven_access_sem, 1);

        sleep(TRAVEL_TIME);

        printf("PID: %d: %s: Dostarczam pizzę: %d\n", getpid(),
               get_current_time(), my_pizza_type);

        sleep(TRAVEL_TIME);
    }
}
