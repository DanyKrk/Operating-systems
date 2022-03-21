//
// Created by danie on 19/03/2022.
//

#include "sys_copy.h"

#include <stdlib.h>
#include <stdio.h>
#include <ctype.h>
#include <string.h>

#include <unistd.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

#define BUFF_SIZE 257

int sys_copy(char *src, char *dest) {
    int source = open(src, O_RDONLY);
    int target = open(dest, O_CREAT | O_WRONLY | O_TRUNC, S_IRUSR | S_IWUSR); // create ifne, wr only, trunc to 0, args with OCR
    char *buff = (char *) malloc(BUFF_SIZE * sizeof(char));

    int read_chars = read(source, buff, (size_t) BUFF_SIZE * sizeof(char));
    int line_start_in_buff = 0;
    int line_started = 0;
    int ctr = 0;

    while(read_chars > 0){
        while(ctr < read_chars){
            if(isspace(buff[ctr])){
                if(buff[ctr] == '\n' && line_started){
                    int line_length = ctr - line_start_in_buff + 1;
                    if(write(target, &buff[line_start_in_buff], (size_t) line_length * sizeof(char)) != line_length * sizeof(char)){
                        perror("write error!!!");
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
            if(write(target, &buff[line_start_in_buff], (size_t) line_length * sizeof(char)) != line_length * sizeof(char)){
                perror("write error!!!");
            }
            line_start_in_buff = 0;
        }
        read_chars = read(source, buff, (size_t) BUFF_SIZE * sizeof(char));
        ctr = 0;
    }
    close(source);
    close(target);
    free(buff);
    return 0;
}
