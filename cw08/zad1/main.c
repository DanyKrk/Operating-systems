#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <pthread.h>
#include <math.h>
#include <sys/time.h>

#define IMAGE_SIZE 256

int** image;
int** negative_image;
int w, h;
int M;
int threads_num;

void load_image(char* filename){
    FILE* file = fopen(filename, "r");
    char* buffer = calloc(IMAGE_SIZE, sizeof(char));

    for (int i = 0; i < 3; i++)  // skip magic number and author
        fgets(buffer, IMAGE_SIZE, file);
    sscanf(buffer, "%d %d\n", &w, &h);  // get dimensions
    fgets(buffer, IMAGE_SIZE, file);
    sscanf(buffer, "%d \n", &M);  // get color variety

    image = calloc(h, sizeof(int*));
    for (int i = 0; i < h; i++)
        image[i] = calloc(w, sizeof(int));

    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            fscanf(file, "%d", &image[i][j]);
        }
    }
    fclose(file);
}

void write_header(FILE* file, char* method) {
    printf("Number of threads: %d\n", threads_num);
    printf("Method used: %s\n", method);
    
    fprintf(file, "Number of threads: %d\n", threads_num);
    fprintf(file, "Method used: %s\n", method);
}

void* numbers_method(void* arg){
    int idx = *((int *) arg);
    struct timeval stop, start;

    int start_color = (M + 1) / threads_num * idx;
    int end_color = ((M + 1) / threads_num * (idx + 1));
    if (idx == threads_num - 1) end_color = M + 1;

    gettimeofday(&start, NULL);  // start measuring time

    for (int i = 0; i < h; i++){
        for (int j = 0; j < w; j++){
            if (start_color <= image[i][j] && image[i][j] < end_color)
                negative_image[i][j] = M - image[i][j];
        }
    }

    gettimeofday(&stop, NULL);  // stop measuring time
    long unsigned int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(&time);
}

void* block_method(void* arg) {
    int idx = *((int *) arg);
    struct timeval stop, start;
    gettimeofday(&start, NULL);  // start measuring time

    int start_x = (idx) * ceil(w / threads_num);
    int end_x = ((idx + 1)* ceil(w / threads_num) - 1);
    if (idx == threads_num - 1) end_x = w - 1;

    for (int i = 0; i < h; i++){
        for (int j = start_x; j <= end_x; j++)
            negative_image[i][j] = M - image[i][j];
    }

    gettimeofday(&stop, NULL);  // stop measuring time

    long unsigned int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    pthread_exit(&time);
}

pthread_t* start_threads(char* method) {
    pthread_t* threads = calloc(threads_num, sizeof(pthread_t));
    int* threads_ids = calloc(threads_num, sizeof(int));

    for(int i = 0; i < threads_num; i++){
        threads_ids[i] = i;
        if (strcmp(method, "numbers") == 0)
            pthread_create(&threads[i], NULL, &numbers_method, &threads_ids[i]);
        else
            pthread_create(&threads[i], NULL, &block_method, &threads_ids[i]);
    }
    return threads;
}

void write_threads_results(pthread_t* threads, FILE* times_file) {
    long unsigned int time;
    for(int i = 0; i < threads_num; i++) {
        pthread_join(threads[i], (void **) time);
        printf("thread id: %3d - time: %lu μs\n", i, time);
        fprintf(times_file, "thread id: %3d - time: %lu μs\n", i, time);
    }
}

void write_total_time(struct timeval start, struct timeval stop, FILE* times_file) {
    long unsigned int time = (stop.tv_sec - start.tv_sec) * 1000000 + stop.tv_usec - start.tv_usec;
    printf("Total time: %lu μs\n", time);
    fprintf(times_file, "Total time: %lu μs\n\n\n", time);
}

void save_negative_image(char* filename) {
    FILE *f = fopen(filename, "w");

    fprintf(f, "P2\n");
    fprintf(f, "%d %d\n", w, h);
    fprintf(f, "%d\n", M);

    for (int i = 0; i < h; i++) {
        for (int j = 0; j < w; j++)
            fprintf(f, "%d ", negative_image[i][j]);
        fprintf(f, "\n");
    }
    fclose(f);
}

int main(int argc, char* argv[]){
    if (argc != 5){
        printf("Wrong arguments number!!!\n");
        return -1;
    }
    threads_num = atoi(argv[1]);
    char* method = argv[2];
    char* input_file = argv[3];
    char* output_file = argv[4];
    struct timeval start, stop;

    load_image(input_file);

    negative_image = calloc(h, sizeof(int *));
    for (int i = 0; i < h; i++) {
        negative_image[i] = calloc(w, sizeof(int));
    }

    FILE* times_file = fopen("times.txt", "a");
    write_header(times_file, method);

    gettimeofday(&start, NULL);

    pthread_t* threads = start_threads(method);
    write_threads_results(threads, times_file);

    gettimeofday(&stop, NULL);
    write_total_time(start, stop, times_file);
    fclose(times_file);

    save_negative_image(output_file);
    for (int i = 0; i < h; i++) {
        free(negative_image[i]);
    }
    free(negative_image);
    free(threads);
    return 0;
}