#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#define MAX_ARGUMENTS 5
#define MAX_COMMANDS_NUM 10
#define LINE_SIZE 256

int extract_commands_and_arguments_from_line(char *commands[MAX_COMMANDS_NUM][MAX_ARGUMENTS + 1], char *line){
    int n = 0;
    char *token;
    while ((token = strtok(n == 0 ? line : NULL, "|")) != NULL) {
        int i = 0;
        char *arg;
        while (token != NULL && i < MAX_ARGUMENTS) {
            arg = strsep(&token, " \n\t");
            if (*arg) commands[n][i++] = arg;

        }
        while (i < MAX_ARGUMENTS) commands[n][i++] = NULL;
        n++;
    }
    return n;
}

int execute_line(char *line) {
    char *commands[MAX_COMMANDS_NUM][MAX_ARGUMENTS + 1];
    int commands_num = extract_commands_and_arguments_from_line(commands, line);

    int prev_pipe_out, fd[2];
    prev_pipe_out = STDIN_FILENO;
    size_t i;
    
    for (i = 0; i < commands_num - 1; i++) {
        pipe(fd);

        if (fork() == 0) {
            // Redirect previous pipe to stdin
            if (prev_pipe_out != STDIN_FILENO) {
                dup2(prev_pipe_out, STDIN_FILENO);
                close(prev_pipe_out);
            }

            // Redirect stdout to current pipe
            dup2(fd[1], STDOUT_FILENO);
            close(fd[1]);

            // Start command
            execvp(commands[i][0], commands[i]);

            perror("execvp failed");
            exit(1);
        }

        // Close read end of previous pipe (not needed in the parent)
        close(prev_pipe_out);

        // Close write end of current pipe (not needed in the parent)
        close(fd[1]);

        // Save read end of current pipe to use in next iteration
        prev_pipe_out = fd[0];
    }

    // Get stdin from last pipe
    if (prev_pipe_out != STDIN_FILENO) {
        dup2(prev_pipe_out, STDIN_FILENO);
        close(prev_pipe_out);
    }

    // Start last command
    execvp(commands[i][0], commands[i]);

    perror("execvp error!!!");
    exit(1);
}

void prepare_line_to_execute(char *line_to_execute, char commands[MAX_COMMANDS_NUM][LINE_SIZE],
                             int commands_num, char *unresolved_line_to_execute){
    for (int i = 0; i < commands_num - 1; ++i) {
        int line_to_concat_id = unresolved_line_to_execute[8 * (i + 1) + i * 4] - '0';
        line_to_concat_id--;
        strcat(line_to_execute, commands[line_to_concat_id]);
        if (i != commands_num - 2) strcat(line_to_execute, " | ");
    }
}

int extract_commands(char commands[MAX_COMMANDS_NUM][LINE_SIZE], char *unresolved_line_to_execute, char *file_name){
    FILE *f = fopen(file_name, "r");
    char *line = NULL;
    size_t len = 0;
    int commands_num = 0;
    while (getline(&line, &len, f) > 0) {
        for (int i = 12; i < strlen(line) + 1; i++) {
            commands[commands_num][i - 12] = line[i];
        }
        commands[commands_num][strcspn(commands[commands_num],"\r\n")] = 0;
        commands_num++;
    }
    fclose(f);
    strcpy(unresolved_line_to_execute, line);
    return commands_num;
}

int main(int argc, char **argv) {
    if (argc != 2) {
        perror("Program should take 1 argument!!!");
        exit(1);
    }
    char *unresolved_line_to_execute = calloc(LINE_SIZE, sizeof(char));
    char commands[MAX_COMMANDS_NUM][LINE_SIZE];
    int commands_num = extract_commands(commands, unresolved_line_to_execute, argv[1]);

    char *line_to_execute = calloc(commands_num * LINE_SIZE, sizeof(char));
    prepare_line_to_execute(line_to_execute, commands, commands_num, unresolved_line_to_execute);
    free(unresolved_line_to_execute);
    execute_line(line_to_execute);
    free(line_to_execute);
    return 0;
}