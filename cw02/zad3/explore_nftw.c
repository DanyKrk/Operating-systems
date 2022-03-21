#define _XOPEN_SOURCE 500
#include <ftw.h>
#include <dirent.h>
#include <string.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <time.h>
#include <stdlib.h>
#include <fcntl.h>

#include "explore_nftw.h"

#define MAX_FD_NUM 10
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

int nftw_print_function(const char *filename, const struct stat *statptr, int fileflags, struct FTW *pfwt){
    print_from_stat(filename, statptr);
    return 0;
}

void explore_nftw(char *path){
    if (path == NULL) return;
    int flags = FTW_PHYS;
    nftw(path, nftw_print_function, MAX_FD_NUM, flags);
    printf("Regular files: %d, Directories: %d, Character devices: %d, Block devices: %d, FIFOs: %d, Slinks: %d, Sockets: %d \n\n\n",
           reg_file_num, dir_num, char_dev_num, block_dev_num, fifo_num, slink_num, socket_num);
}
