#include "header.h"

sem_t *sem_addr[SEM_NUM];
char sem_names[SEM_NUM][NAME_SIZE];
int oven_shm_fd, table_shm_fd;
Oven *oven;
Table *table;

void exit_procedure(){
    printf("Exit procedure\n");
    munmap(oven, sizeof(oven));
    munmap(table, sizeof(table));
    shm_unlink(OVEN_NAME);
    shm_unlink(TABLE_NAME);
    for(int i = 0; i < SEM_NUM; i++){
        sem_close(sem_addr[i]);
    }
    for(int i = 0; i < SEM_NUM; i++){
        sem_unlink(sem_names[i]);
    }
}

void create_shm(){
    oven_shm_fd = shm_open(OVEN_NAME, O_RDWR | O_CREAT, 0666);
    table_shm_fd = shm_open(TABLE_NAME, O_RDWR | O_CREAT, 0666);

    ftruncate(oven_shm_fd, sizeof(oven));
    ftruncate(table_shm_fd, sizeof(table));

    oven = mmap(NULL, sizeof(oven), PROT_READ | PROT_WRITE, MAP_SHARED, oven_shm_fd, 0);
    table = mmap(NULL, sizeof(table), PROT_READ | PROT_WRITE, MAP_SHARED, table_shm_fd, 0);

    printf("Created oven (fd: %d) and table (fd: %d)\n", oven_shm_fd, table_shm_fd);
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
    for(int i = 0; i < SEM_NUM; i++){
        int value;
        switch (i) {
            case OVEN_ACCESS_SEM_ID:
            case TABLE_ACCESS_SEM_ID:
                value = 1;
                break;
            case FULL_OVEN_SEM_ID:
                value = OVEN_SIZE;
                break;
            case FULL_TABLE_SEM_ID:
                value = TABLE_SIZE;
                break;
            case EMPTY_TABLE_SEM_ID:
                value = 0;
                break;
        }
        sem_addr[i] = sem_open(sem_names[i], O_RDWR | O_CREAT, 0666, value);
        if(sem_addr[i] == SEM_FAILED) {
            perror("Problem with creating semaphore");
            exit(1);
        }
    }
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
    fill_sem_names(sem_names);
    create_sem();

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
