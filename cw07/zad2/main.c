#include "header.h"


sem_t oven_shm_id, table_shm_id, sem_set_id;
Oven *oven;
Table *table;

void exit_procedure(){
    printf("Exit procedure\n");
    shmdt(oven);
    shmdt( table);
    shmctl(oven_shm_id, IPC_RMID, NULL);
    shmctl(table_shm_id, IPC_RMID, NULL);
    semctl(sem_set_id, 0, IPC_RMID, NULL);
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

    printf("Created oven (id: %d) and table (id: %d)\n", oven_shm_id, table_shm_id);
}

void initialize_oven(){
    oven -> next_in_id = 0;
    oven -> pizzas_num = 0;
}

void initialize_table(){
    table -> next_in_id = 0;
    table -> pizzas_num = 0;
}

void create_sem(){
    key_t sem_key;

    char *home_path = get_home_path();
    sem_key = ftok(home_path, SEM_PROJ_ID);
    if(sem_key == -1) {
        perror("Problem with getting semaphore key\n");
        exit(1);
    }

    sem_set_id = semget(sem_key, 5, IPC_CREAT | 0666);
    if(sem_set_id == -1) {
        perror("Problem with creating semaphore");
        exit(1);
    }
    printf("Created semaphore set (id: %d)\n", sem_set_id);
}

void initialize_sem(){
    union semun arg;
    arg.val = 1;

    semctl(sem_set_id, OVEN_ACCESS_SEM_ID, SETVAL, arg);
    semctl(sem_set_id, TABLE_ACCESS_SEM_ID, SETVAL, arg);

    arg.val = OVEN_SIZE;
    semctl(sem_set_id, FULL_OVEN_SEM_ID, SETVAL, arg);

    arg.val = TABLE_SIZE;
    semctl(sem_set_id, FULL_TABLE_SEM_ID, SETVAL, arg);

    arg.val = 0;
    semctl(sem_set_id, EMPTY_TABLE_SEM_ID, SETVAL, arg);
}

int main(int argc, char *argv[]){
    if(argc != 3){
        fprintf(stderr, "Wrong number of arguments!!!");
        exit(1);
    }
    int bakers_num = atoi(argv[1]);
    int deliverers_num = atoi(argv[2]);

    create_shm();
    initialize_oven();
    initialize_table();
    create_sem();
    initialize_sem();

    signal(SIGINT, exit_procedure);

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

    for(int i = 0; i < bakers_num + deliverers_num; i++) wait(NULL);
}
