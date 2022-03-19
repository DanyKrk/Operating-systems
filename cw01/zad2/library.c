//
// Created by danie on 13/03/2022.
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <assert.h>
#include "library.h"

#define COMMAND_SIZE 1000

char* temp_filename = "temp.txt";


char*** create_table(int table_size){
     return (char***) calloc(table_size, sizeof(char** ));
}

int* create_block_sizes(int table_size){
    return (int* ) calloc(table_size, sizeof(int));
}

void wc_files(int files_num, char** file_names){
    char command [COMMAND_SIZE] = "wc";
    for(int i = 0; i < files_num; i++){
        strcat(command, " ");
        strcat(command, file_names[i]);
    }

    strcat(command, " > ");
    strcat(command, "trash.txt");

    if(system(command) == -1){
        fprintf (stderr, "Couldn't execute command!!!");
    }
}

void wc_files_to_tmp(int files_num, char** file_names){
    char command [COMMAND_SIZE] = "wc";
    for(int i = 0; i < files_num; i++){
        strcat(command, " ");
        strcat(command, file_names[i]);
    }

    strcat(command, " > ");
    strcat(command, temp_filename);
    if(system(command) == -1){
        fprintf (stderr, "Couldn't execute command!!!");
    }
}

int find_free_index_in_table(int table_size, char*** table){
    for(int i = 0; i < table_size; i++){
        if(table[i] == 0){
            return i;
        }
    }
    return -1;
}

int save_data_from_tmp_to_table(int table_size, char*** table, int* block_sizes, int rows_num){
    int free_index_in_table = find_free_index_in_table(table_size, table);
    if(free_index_in_table == -1){
        fprintf (stderr, "Table is full, no free space for new block!!!");
    }
    table[free_index_in_table] = (char**) calloc(rows_num, sizeof(char*));
    block_sizes[free_index_in_table] = rows_num;


    FILE* fp;
    char* buffer = NULL;
    size_t buffer_len = 0;
    fp = fopen(temp_filename, "r");
    if (fp == NULL) {
        fprintf (stderr, "Couldn't open file: %s", temp_filename);
        exit(EXIT_FAILURE);
    }

    for(int row_cntr = 0; row_cntr < rows_num; row_cntr++){
        int line_len = getline(&buffer, &buffer_len, fp) + 1;
        table[free_index_in_table][row_cntr] = (char*) calloc(line_len, sizeof(char));
        strcpy (table[free_index_in_table][row_cntr], buffer);
    }

    fclose(fp);
    if (buffer) free(buffer);

    return free_index_in_table;
}

void delete_block(int table_size, char*** table, int* block_sizes, int id){
    assert(id < table_size || id >= 0);
    assert(table[id] != NULL && block_sizes[id] > 0);

    for(int i = 0; i < block_sizes[id]; i++){
        if(table[id][i] != NULL) free(table[id][i]);
    }
    free(table[id]);
    table[id] = NULL;
    block_sizes[id] = 0;
}

void delete_table(int* table_size, char*** table, int* block_sizes){
    assert(table != NULL && *table_size != 0);
    assert(block_sizes != NULL);
    for(int i = 0; i < *table_size; i++){
        if(table[i] != NULL) delete_block(*table_size, table, block_sizes, i);
    }
    free(table);
    free(block_sizes);
    *table_size = 0;
}

int wc_rows_num(int files_num){
    if(files_num == 1){
        return 1;
    }
    else{
        return files_num + 1;
    }
}

void print_table(int table_size, char **const *table, const int *block_sizes) {
    for(int i = 0; i < table_size; i++){
        printf("Block nr. %d: \n", i);
        if (table[i] == NULL) {
            printf("NULL \n");
        }
        for(int j = 0; j < block_sizes[i]; j++){
            printf("%s \n", table[i][j]);
        }
    }
}