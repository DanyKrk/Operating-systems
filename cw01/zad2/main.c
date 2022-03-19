#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <assert.h>
#include "library.h"
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

clock_t st_time, en_time;
struct tms st_cpu, en_cpu;
char* report_file_name = "raport2.txt";




void save_timer(char *name, FILE *f){
    int clk_tics = sysconf(_SC_CLK_TCK);
    double real_time = (double)(en_time - st_time) / clk_tics;
    double user_time = (double)(en_cpu.tms_utime - st_cpu.tms_utime) / clk_tics;
    double system_time = (double)(en_cpu.tms_stime - st_cpu.tms_stime) / clk_tics;
    printf("%40s:\t\t%15f\t%15f\t%15f\t\n",
            name,
            real_time,
            user_time,
            system_time);
    fprintf(f, "%40s:\t\t%15f\t%15f\t%15f\t\n",
            name,
            real_time,
            user_time,
            system_time);
}

void write_report_header(FILE *f){
    printf("%40s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "Real [s]",
            "User [s]",
            "System [s]");
    fprintf(f, "%40s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "Real [s]",
            "User [s]",
            "System [s]");
}

int is_command(int commands_num, char** commands, char* candidate){
    for(int i = 0; i < commands_num; i++){
        if(strcmp(commands[i], candidate) == 0){
            return 1;
        }
    }
    return 0;
}

int main(int argc, char** argv) {
    FILE* report_file = fopen(report_file_name, "a");
    write_report_header(report_file);

    int commands_num = 8;
    char* commands[] = {"create_table", "wc_files", "wc_and_save", "remove_block", "start_timer", "stop_timer", "save_timer", "remove_table"};
    int table_size;
    char*** table = NULL;
    int* block_sizes = NULL;
    int files_num;
    char** file_names = NULL;


    int cnt = 1;
    while(cnt < argc){
        if(strcmp(argv[cnt], "create_table") == 0){
            cnt++;
            assert(cnt < argc);
            assert(!is_command(commands_num, commands, argv[cnt]));
            if(table != NULL && table_size > 0) delete_table(&table_size, table, block_sizes);
            table_size = atoi(argv[cnt]);
            table = create_table(table_size);
            block_sizes = create_block_sizes(table_size);
            cnt++;
            continue;
        }

        if(strcmp(argv[cnt], "wc_files") == 0){
            int wc_command_id = cnt;
            cnt++;
            files_num = 0;
            while (cnt < argc && !is_command(commands_num, commands, argv[cnt])){
                files_num += 1;
                cnt++;
            }
            assert(files_num > 0);

            if(file_names != NULL) free(file_names);
            file_names = (char**) calloc(files_num, sizeof(char*));

            int saved_filenames_cnt, filename_id;
            for(saved_filenames_cnt = 0, filename_id = wc_command_id + 1 ; saved_filenames_cnt < files_num ; saved_filenames_cnt++, filename_id++){
                file_names[saved_filenames_cnt] = argv[filename_id];
            }

            wc_files(files_num, file_names);
            free(file_names);
            file_names = NULL;
            continue;
        }

        if(strcmp(argv[cnt], "wc_and_save") == 0){
            assert(table != NULL && table_size > 0);
            int wc_command_id = cnt;
            cnt++;
            files_num = 0;
            while (cnt < argc && !is_command(commands_num, commands, argv[cnt])){
                files_num += 1;
                cnt++;
            }
            assert(files_num > 0);

            if(file_names != NULL) free(file_names);
            file_names = (char**) calloc(files_num, sizeof(char*));

            int saved_filenames_cnt, filename_id;
            for(saved_filenames_cnt = 0, filename_id = wc_command_id + 1 ; saved_filenames_cnt < files_num ; saved_filenames_cnt++, filename_id++){
                file_names[saved_filenames_cnt] = argv[filename_id];
            }

            wc_files_to_tmp(files_num, file_names);
            save_data_from_tmp_to_table(table_size, table, block_sizes, wc_rows_num(files_num));
            free(file_names);
            file_names = NULL;
            continue;
        }

        if(strcmp(argv[cnt], "remove_block") == 0){
            assert(table != NULL);
            cnt++;
            assert(cnt < argc);
            assert(!is_command(commands_num,commands, argv[cnt]));
            int block_id = atoi(argv[cnt]);
            delete_block(table_size, table, block_sizes, block_id);
            cnt ++;
            continue;
        }

        if(strcmp(argv[cnt], "start_timer") == 0){
            cnt ++;
            st_time = times(&st_cpu);
            continue;
        }

        if(strcmp(argv[cnt], "stop_timer") == 0){
            cnt++;
            en_time = times(&en_cpu);
            continue;
        }

        if(strcmp(argv[cnt], "save_timer") == 0){
            cnt++;
            assert(cnt < argc);
            assert(!is_command(commands_num, commands, argv[cnt]));
            char* record_name = argv[cnt];
            save_timer(record_name, report_file);
            cnt ++;
            continue;
        }

        if(strcmp(argv[cnt], "remove_table") == 0){
            cnt++;
            assert(table != NULL);
            delete_table(&table_size, table, block_sizes);
            continue;
        }
    }

    if(table != NULL && table_size > 0) delete_table(&table_size, table, block_sizes);
    return 0;
}
