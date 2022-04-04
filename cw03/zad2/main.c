#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/times.h>
#include <time.h>
#include <unistd.h>
#include <sys/resource.h>
#include <sys/wait.h>

#define FILENAME_SIZE 100
#define REPORT_DESCRIPTION_SIZE 100
#define BUFFER_SIZE 50
#define INTERVAL_START 0.0
#define INTERVAL_END 1.0

clock_t st_time, en_time;
struct tms st_cpu, en_cpu;
char* report_file_name = "pomiar_zad_2.txt";


void save_timer(char *name, FILE *f){
    long long clk_tics = sysconf(_SC_CLK_TCK);
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
    printf( "%72s\t\t%15s\t%15s\t%15s\n",
            "Name",
            "Real [s]",
            "User [s]",
            "System [s]");
    fprintf(f, "%72s\t\t%15s\t%15s\t%15s\n",
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

double integrated_function(double x){
    return 4/(x * x + 1);
}

int main(int argc, char **argv){
    FILE* report_file = fopen(report_file_name, "a");
    double rect_width;
    long long processes_num, rect_num, rect_num_per_process, extra_rect_num;
    if(argc != 3){
        fprintf(stderr, "This program should take 2 arguments!!!");
        return 1;
    }
    else {
        rect_width = strtod(argv[1], NULL);
        processes_num = atoi(argv[2]);
    }
    rect_num = (long long) ((INTERVAL_END - INTERVAL_START) / rect_width) + 0.5;
    if(rect_num == 0) rect_num = 1;
    if(processes_num >= rect_num){
        fprintf(stderr, "There shouldn't be more processes than rectangles!!!");
        return 1;
    }
    rect_num_per_process = rect_num/processes_num;
    extra_rect_num = rect_num % processes_num;
    long long curr_rect_num_per_process;
    double curr_distinguished_point = rect_width/2;
    pid_t *child_processes_ids = (pid_t *)calloc(processes_num, sizeof(pid_t));

    start_timer();
    for(long long i = 0; i<processes_num; i++) {
        curr_rect_num_per_process = rect_num_per_process;
        if(extra_rect_num > 0){
            curr_rect_num_per_process += 1;
            extra_rect_num -= 1;
        }


        pid_t child_pid;
        child_pid = fork();
        if (child_pid == 0) {
            double sum = 0;
            for(long long r = 0; r < curr_rect_num_per_process; r++){
                sum += integrated_function(curr_distinguished_point) * rect_width;
                curr_distinguished_point += rect_width;
            }
            char tmp_file_name [FILENAME_SIZE];
            sprintf(tmp_file_name, "w%lld.txt", i+1);

            FILE* tmp_file = fopen(tmp_file_name, "w");
            fprintf(tmp_file, "%.15lf",sum);
            fclose(tmp_file);
            return 0;
        } else{
            curr_distinguished_point +=  curr_rect_num_per_process * rect_width;
            child_processes_ids[i] = child_pid;
        }
    }

    for(long long i = 0; i < processes_num; i++){
        waitpid(child_processes_ids[i], NULL,0);
    }

    double result = 0.0;
    for(long long i = 1; i <= processes_num; i++){
        char tmp_file_name [FILENAME_SIZE];
        sprintf(tmp_file_name, "w%lld.txt", i);
        FILE* tmp_file = fopen(tmp_file_name, "r");
        double tmp_res;
        fscanf(tmp_file, "%lf",&tmp_res);
        fclose(tmp_file);
        result += tmp_res;
    }
    stop_timer();
    printf("Result: %.15lf\n", result);
    char report_description [REPORT_DESCRIPTION_SIZE];
    sprintf(report_description, "Operation time for rectangles of width: %.15lf and %lld processes", rect_width, processes_num);
    write_report_header(report_file);
    save_timer(report_description, report_file);
    fclose(report_file);
    free(child_processes_ids);
    return 0;
}
