#include "lib_count.h"

#include <stdlib.h>
#include <stdio.h>

#define BUFF_SIZE 257

void lib_count(char* src_name, char ch){
    FILE *source = fopen(src_name, "r");
    int chars_count = 0;
    int rows_count = 0;

    char *buff = (char *) calloc(BUFF_SIZE, sizeof(char));
    int read_chars = fread(buff, sizeof(char), (size_t) BUFF_SIZE, source);
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
        read_chars = fread(buff, sizeof(char), (size_t) BUFF_SIZE, source);
        cnt = 0;
    }
    printf("Chars num: %d, Rows num: %d \n", chars_count, rows_count);
    fclose(source);
    free(buff);
}