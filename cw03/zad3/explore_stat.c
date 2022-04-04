#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>

#include "explore_stat.h"

#define MAX_PATH_LEN 256


void explore_stat(char *absolute_path, char* relative_path, char *searched_string, int max_depth, int curr_depth){
    if (absolute_path == NULL) return;

    DIR *dir = opendir(absolute_path);
    if (dir == NULL){
        perror("Problem with opening directory \n");
        exit(10);
    }

    struct dirent *file;
    struct stat file_stats;
    while ((file = readdir(dir)) != NULL){
        char next_absolute_path[MAX_PATH_LEN];
        strcpy(next_absolute_path, absolute_path);
        strcat(next_absolute_path, "/");
        strcat(next_absolute_path, file->d_name);

        char next_relative_path[MAX_PATH_LEN];
        strcpy(next_relative_path, relative_path);
        strcat(next_relative_path, "/");
        strcat(next_relative_path, file->d_name);

        if (lstat(next_absolute_path, &file_stats) < 0){
            perror("Problem with lstating file");
            exit(10);
        }

        if (S_ISDIR(file_stats.st_mode) && curr_depth != max_depth){
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                continue;
            }
            pid_t child_pid;
            child_pid = fork();
            if(child_pid == 0) {
                explore_stat(next_absolute_path, next_relative_path, searched_string, max_depth, curr_depth + 1);
                return;
            }
        }
        else if (S_ISREG(file_stats.st_mode)){
            FILE *file_pointer = fopen(next_absolute_path, "r");
            char *line = NULL;
            size_t len = 0;
            ssize_t read;

            if (!file_pointer) {
                fprintf(stderr, "Problem with opening %s\n", next_absolute_path);
                return;
            }

            while ((read = getline(&line, &len, file_pointer)) != -1) {
                line[strcspn(line, "\n")] = 0;
                if (strcmp(line, searched_string) == 0) {
                    printf("Process %d found searched string at %s\n", getpid(), next_relative_path);
                    break;
                }
            }
            if (line) {
                free(line);
            }
            fclose(file_pointer);
        }
    }
    closedir(dir);
}