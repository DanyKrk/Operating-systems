#include "explore_stat.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>

#define FILENAME_SIZE 256



int main(int argc, char **argv){
    if(argc != 4){
        fprintf(stderr, "Wrong number of parameters");
        exit(10);
    }

    char absolute_path[FILENAME_SIZE];
    if(realpath(argv[1], absolute_path) == NULL){
        perror("Problem with finding realpath!!!");
        exit(10);
    }
    char *searched_string = argv[2];
    int max_depth = atoi(argv[3]);
    char relative_path[] = "";

    explore_stat(absolute_path,relative_path, searched_string, max_depth, 0);

    return 0;
}
