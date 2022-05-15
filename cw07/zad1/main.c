#include "header.h"


int oven_shm_id, table_shm_id, sem_id;
Oven *oven;
Table *table;

void exit_procedure(){
    printf("Exit procedure\n");
    shmdt(oven);
    shmdt( table);
    shmctl(oven_shm_id, IPC_RMID, NULL);
    shmctl(table_shm_id, IPC_RMID, NULL);
    semctl(sem_id, 0, IPC_RMID);
}

void initialize_oven(){
    oven -> next_in_id = 0;
    oven -> next_out_id = 0;
    oven -> pizzas_num = 0;
}

void initialize_table(){
    table -> next_in_id = 0;
    table -> next_out_id = 0;
    table -> pizzas_num = 0;
}

void create_shm(){
    key_t oven_key, table_key;

    char *home_path = get_home_path();
    oven_key = ftok(home_path, OVEN_PROJ_ID);
    table_key = ftok(home_path, TABLE_PROJ_ID);

    oven_shm_id = shmget(oven_key, sizeof(Oven), IPC_CREAT | 0666);
    table_shm_id = shmget(table_key, sizeof(Table), IPC_CREAT | 0666);

    oven = shmat(oven_shm_id, NULL, 0);
    table = shmat(table_shm_id, NULL, 0);

    initialize_oven(oven);
    initialize_table(table);

    printf("Created oven (id: %d) and table (id: %d)\n", oven_shm_id, table_shm_id);
}

void create_sem(){
    key_t sem_key;

    char *home_path = get_home_path();
    sem_key = ftok(home_path, SEM_PROJ_ID);

    sem_id = semget(sem_key, 2, IPC_CREAT | 0666);

    union semun arg;
    arg.val = 1;

    semctl(sem_id, OVEN_SEM_ID, SETVAL, arg);
    semctl(sem_id, TABLE_SEM_ID, SETVAL, arg);
    printf("Created semaphore set (id: %d)\n", sem_id);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Wrong number of arguments!!!");
        exit(1);
    }
    int bakers_num = atoi(argv[1]);
    int deliverers_num = atoi(argv[2]);

    atexit(exit_procedure);
    create_shm();
    create_sem();

    for(int i = 0; i < bakers_num; i++){
        pid_t child_id = fork();
        if(child_id == 0){
            execl("./baker", "./baker", NULL);
        }
    }

    for(int i = 0; i < deliverers_num; i++){
        pid_t child_id = fork();
        if(child_id == 0){
            execl("./deliverer", "./deliverer", NULL);
        }
    }
}
