//
// Created by danie on 13/03/2022.
//

#ifndef LAB1_LIBRARY_H
#define LAB1_LIBRARY_H

char*** create_table(int table_size);

int* create_block_sizes(int table_size);

void wc_files(int files_num, char** file_names);

void wc_files_to_tmp(int files_num, char** file_names);

int save_data_from_tmp_to_table(int table_size, char*** table, int* block_sizes, int rows_num);

void delete_block(int table_size, char*** table, int* block_sizes, int id);

void delete_table(int* table_size, char*** table, int* block_sizes);

int wc_rows_num(int files_num);

void print_table(int table_size, char **const *table, const int *block_sizes);

#endif //LAB1_LIBRARY_H
