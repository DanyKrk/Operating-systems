#include <stdlib.h>
#include <stdio.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#include "sys_count.h"

#define BUFF_SIZE 257

void sys_count(char* src_name, char ch){
    int source = open(src_name, O_RDONLY);
    int chars_count = 0;
    int rows_count = 0;

    char *buff = (char *) calloc(BUFF_SIZE, sizeof(char));
    int read_chars = read(source, buff, (size_t) BUFF_SIZE * sizeof(char));
    int cnt = 0;
    int curr_row = 0;
    int last_ch_row = -1;

    while (read_chars > 0){
        while(cnt < read_chars){
            if(buff[cnt] == ch){
                chars_count++;
                if(curr_row != last_ch_row){
                    rows_count++;
                    last_ch_row = curr_row;
                }
            }
            else if(buff[cnt] == '\n') curr_row++;
            cnt++;
        }
        read_chars = read(source, buff, (size_t) BUFF_SIZE * sizeof(char));
        cnt = 0;
    }
    printf("Chars num: %d, Rows num: %d \n", chars_count, rows_count);
    close(source);
    free(buff);
}