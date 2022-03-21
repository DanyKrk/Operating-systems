#include "explore_stat.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FILENAME_SIZE 256



int main(int argc, char **argv){
    if(argc != 2){
        printf("No path specified!!!");
        exit(10);
    }

    char path[FILENAME_SIZE];
    if(realpath(argv[1], path) == NULL){
        perror("Problem with finding realpath!!!");
        exit(10);
    }

    explore_stat(path, 1);

    return 0;
}
