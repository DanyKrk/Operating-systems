//
// Created by daniel on 14.05.22.
//

#ifndef CW07_HEADER_H
#define CW07_HEADER_H

#include <pwd.h>
#include <unistd.h>
#include "stdio.h"
#include "stdlib.h"
#include "sys/sem.h"
#include "sys/ipc.h"
#include "sys/types.h"
#include "sys/shm.h"

#define TABLE_SIZE 5
#define OVEN_SIZE 5
#define OVEN_PROJ_ID 2115
#define TABLE_PROJ_ID 2137

#define OVEN_PATH "oven"
#define TABLE_PATH "table"


typedef struct {
    int pizzas[TABLE_SIZE];
    int pizzas_num;
    int next_in_id;
    int next_out_id;
} Table;

typedef struct {
    int pizzas[OVEN_SIZE];
    int pizzas_num;
    int next_in_id;
    int next_out_id;
} Oven;

char* get_home_path(){
    char* path = getenv("HOME");
    if (path == NULL)
        path = getpwuid(getuid())->pw_dir;
    return path;
}

#endif //CW07_HEADER_H
