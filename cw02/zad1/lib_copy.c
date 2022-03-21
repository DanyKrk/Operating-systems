#include "lib_copy.h"
#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#define BUFF_SIZE 257

int whitespaces_only(char *str, int len){
    for(int i = 0; i < len; i++){
        if(isspace(str[i])) return 1;
    }
    return 0;
}

int lib_copy(char *src, char *dest) {
    FILE *source = fopen(src, "r");
    FILE *target = fopen(dest, "w+");
    char *buff = (char *) malloc(BUFF_SIZE * sizeof(char));

    int read_chars = fread(buff, sizeof(char), (size_t) BUFF_SIZE, source);
    int line_start_in_buff = 0;
    int line_started = 0;
    int ctr = 0;

    while(read_chars > 0){
        while(ctr < read_chars){
            if(isspace(buff[ctr])){
                if(buff[ctr] == '\n' && line_started){
                    int line_length = ctr - line_start_in_buff + 1;
                    if(fwrite(&buff[line_start_in_buff], sizeof(char), (size_t) line_length, target) != line_length){
                        perror("fwrite error!!!");
                    }
                    line_started = 0;
                }
            }
            else if(!line_started){
                line_start_in_buff = ctr;
                line_started = 1;
            }
            ctr++;
            continue;
        }
        if(line_started){
            int line_length = ctr - line_start_in_buff;
            if(fwrite(&buff[line_start_in_buff], sizeof(char), (size_t) line_length, target) != line_length){
                perror("fwrite error!!!");
            }
            line_start_in_buff = 0;
        }
        read_chars = fread(buff, sizeof(char), (size_t) BUFF_SIZE, source);
        ctr = 0;
    }
    fclose(source);
    fclose(target);
    free(buff);
    return 0;
}
