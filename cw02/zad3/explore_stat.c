#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>

#include "explore_stat.h"

#define MAX_PATH_LEN 256
#define MAX_TIME_LEN 200
const char time_format[] = "%d-%m-%Y  %H:%M:%S";
int reg_file_num = 0;
int dir_num = 0;
int char_dev_num = 0;
int block_dev_num = 0;
int fifo_num = 0;
int slink_num = 0;
int socket_num = 0;


void print_from_stat(const char *file_path, const struct stat *ptr_to_stat){
    char file_type[64] = "undefined";

    if (S_ISREG(ptr_to_stat->st_mode)) {
        strcpy(file_type, "file");
        reg_file_num++;
    }
    else if (S_ISDIR(ptr_to_stat->st_mode)) {
        strcpy(file_type, "dir");
        dir_num++;
    }
    else if (S_ISCHR(ptr_to_stat->st_mode)) {
        strcpy(file_type, "char dev");
        char_dev_num++;
    }
    else if (S_ISBLK(ptr_to_stat->st_mode)) {
        strcpy(file_type, "block dev");
        block_dev_num++;
    }
    else if (S_ISFIFO(ptr_to_stat->st_mode)) {
        strcpy(file_type, "fifo");
        fifo_num++;
    }
    else if (S_ISLNK(ptr_to_stat->st_mode)) {
        strcpy(file_type, "slink");
        slink_num++;
    }
    else if (S_ISSOCK(ptr_to_stat->st_mode)) {
        strcpy(file_type, "socket");
        socket_num++;
    }

    struct tm modif_tm;
    localtime_r(&ptr_to_stat->st_mtime, &modif_tm);
    char modif_str[MAX_TIME_LEN];
    strftime(modif_str, MAX_TIME_LEN, time_format, &modif_tm);

    struct tm access_tm;
    localtime_r(&ptr_to_stat->st_atime, &access_tm);
    char access_str[MAX_TIME_LEN];
    strftime(access_str, MAX_TIME_LEN, time_format, &access_tm);

    printf("Path: %s, links number: %ld, type: %s, size: %ld, last access time: %s, last modification time: %s\n",
           file_path, ptr_to_stat->st_nlink, file_type, ptr_to_stat->st_size, access_str, modif_str);
}

void explore_stat(char *path, int is_first){
    if (path == NULL) return;

    DIR *dir = opendir(path);
    if (dir == NULL){
        perror("Problem with opening directory \n");
        exit(10);
    }

    struct dirent *file;
    struct stat file_stats;
    if (lstat(path, &file_stats) < 0){
        perror("Problem with lstating file");
        exit(10);
    }
    print_from_stat(path, &file_stats);

    while ((file = readdir(dir)) != NULL){
        char next_path[MAX_PATH_LEN];
        strcpy(next_path, path);
        strcat(next_path, "/");
        strcat(next_path, file->d_name);

        if (lstat(next_path, &file_stats) < 0){
            perror("Problem with lstating file");
            exit(10);
        }

        if (S_ISDIR(file_stats.st_mode)){
            if (strcmp(file->d_name, ".") == 0 || strcmp(file->d_name, "..") == 0){
                continue;
            }
            explore_stat(next_path, 0);
        }
        else print_from_stat(next_path, &file_stats);
    }
    closedir(dir);
    if(is_first)
        printf("Regular files: %d, Directories: %d, Character devices: %d, Block devices: %d, FIFOs: %d, Slinks: %d, Sockets: %d \n\n\n",
           reg_file_num, dir_num, char_dev_num, block_dev_num, fifo_num, slink_num, socket_num);
}