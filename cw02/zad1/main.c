#include "sys_copy.h"
#include "lib_copy.h"

#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

#define FILENAME_SIZE 256

clock_t st_time, en_time;
struct tms st_cpu, en_cpu;
char* report_file_name = "pomiar_zad_1.txt";


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
    fprintf(f, "%40s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "Real [s]",
            "User [s]",
            "System [s]");
}

void start_timer(){
st_time = times(&st_cpu);
}

void stop_timer(){
en_time = times(&en_cpu);
}

int main(int argc, char **argv){
    FILE* report_file = fopen(report_file_name, "w");
    write_report_header(report_file);

    char *src_name = (char *) calloc(FILENAME_SIZE, sizeof(char));
    char *dest_name = (char *) calloc(FILENAME_SIZE, sizeof(char));
    if(argc != 3){
        printf("Enter source file name: ");
        scanf ("%256s",src_name);
        printf("Enter destination file name: ");
        scanf ("%256s",dest_name);
    }
    else {
        src_name = argv[1];
        dest_name = argv[2];
    }
    for(int i = 0; i < 10; i++) {
        start_timer();
        lib_copy(src_name, dest_name);
        stop_timer();
        save_timer("Copying with library functions: ", report_file);

        start_timer();
        sys_copy(src_name, dest_name);
        stop_timer();
        save_timer("Copying with system functions: ", report_file);
    }
    fclose(report_file);
    free(src_name);
    free(dest_name);
}
