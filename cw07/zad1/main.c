#include "header.h"


int oven_shm_id, table_shm_id, sem_id;

void exit_procedure(){
    printf("Exit procedure");
    shmdt(oven_shm_id);
    shmdt( table_shm_id);
    shmctl(oven_shm_id, IPC_RMID, NULL);
    shmctl(table_shm_id, IPC_RMID, NULL);
}

void initialize_oven(Oven *oven){
    oven -> next_in_id = 0;
    oven -> next_out_id = 0;
    oven -> pizzas_num = 0;
}

void initialize_table(Table *table){
    table -> next_in_id = 0;
    table -> next_out_id = 0;
    table -> pizzas_num = 0;
}

void create_shm_segment(){
    key_t oven_key, table_key;

    oven_key = ftok(OVEN_PATH, OVEN_PROJ_ID);
    table_key = ftok(TABLE_PATH, TABLE_PROJ_ID);

    oven_shm_id = shmget(oven_key, sizeof(Oven), IPC_CREAT | 0666);
    table_shm_id = shmget(table_key, sizeof(Table), IPC_CREAT | 0666);

    Oven *oven = shmat(oven_shm_id, NULL, 0);
    Table *table = shmat(table_shm_id, NULL, 0);

    initialize_oven(oven);
    initialize_table(table);

    printf("Created oven (id: %d) and table (id: %d)\n", oven_shm_id, table_shm_id);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Wrong number of arguments!!!");
        exit(1);
    }
    int bakers_num = atoi(argv[1]);
    int deliverers_num = atoi(argv[2]);

    create_shm_segment();
    atexit(exit_procedure);
}
