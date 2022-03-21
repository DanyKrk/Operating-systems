#include "lib_count.h"
#include "sys_count.h"

#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>

#define FILENAME_SIZE 256

clock_t st_time, en_time;
struct tms st_cpu, en_cpu;
char* report_file_name = "pomiar_zad_2.txt";


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

    char src_name[FILENAME_SIZE];
    char ch;
    if(argc != 3){
        printf("Enter char to check: ");
        scanf (" %c",&ch);
        printf("Enter source file name: ");
        scanf ("%s",src_name);
    }
    else {
        ch = argv[1][0];
        strcpy(src_name, argv[2]);
    }

    for(int i = 0; i<10; i++) {
        start_timer();
        lib_count(src_name, ch);
        stop_timer();
        save_timer("Counting with library functions: ", report_file);

        start_timer();
        sys_count(src_name, ch);
        stop_timer();
        save_timer("Counting with system functions: ", report_file);
    }

    fclose(report_file);
    return 0;
}
