#include <stdio.h>
#include <string.h>
#include <errno.h>
#include <stdlib.h>


void send_mail(char* address, char* topic, char* content) {
    char *mail = malloc(sizeof(address) + sizeof(topic) + sizeof(content) + sizeof(char ) * 20);
    sprintf(mail, "%s\t%s\t%s\n", address, topic, content);

    char *command = "cat >> mails.txt";

    FILE* cat_input = popen(command, "w");
    if (cat_input == NULL){
        perror("Popen failed!!!");
        exit(1);
    }

    fputs(mail, cat_input);
    free(mail);
    pclose(cat_input);
}

int show_mails(char* mode){
    char* command;
    if (strcmp(mode,"date") == 0){
        command = "cat mails.txt";
    }
    if (strcmp(mode,"sender") == 0){
        command = "cat mails.txt | sort -k1";
    }

    FILE* mails_file = popen(command, "r");
    if (mails_file == NULL){
        perror("Popen failed!!!");
        exit(1);
    }

    char *line;
    size_t len = 0;
    while(getline(&line, &len, mails_file) != -1) {
        printf("%s", line);
    }
    free(line);
    pclose(mails_file);
    return 0;
}

//I wrote mails in text file in this format:
//address \t topic \t content

//they are written in order they arrived
int main(int argc, char **argv) {
    if (argc != 2 && argc != 4) {
        perror("Program should take 1 or 3 arguments!!!");
        exit(1);
    }
    if(argc == 2){
        show_mails(argv[1]);
    }

    if(argc == 4){
        send_mail(argv[1], argv[2], argv[3]);
    }

    return 0;
}