#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include <sys/wait.h>
#include <termios.h>
#include "all_functions.h"

void bring_to_fg(pid_t pid, int pgid){
    tcsetpgrp(STDIN_FILENO, pgid);
    if (kill(pid, SIGCONT) == -1){
        print_red("failed to resume process");
    }
    int status;
    waitpid(pid, &status, WUNTRACED);
    tcsetpgrp(STDIN_FILENO, getpgrp());
}

void bring_to_bg(pid_t pid){
    if (kill(pid, SIGCONT) == -1) {
        print_red("failed to resume process");
        return;
    }
}


void fg_bg_handler(char* command){
    int num_tokens = token_count(command);
    if (num_tokens > 2){
        print_red("invalid input");
    }
    char** tokens = input_to_tokens(command, num_tokens);
    int pid = atoi(tokens[1]);
    int pgid = getpgid(pid);
    if (!strcmp(tokens[0], "fg")){
        bring_to_fg(pid, pgid);
    }
    else if (!strcmp(tokens[0], "bg")){
        bring_to_bg(pid);
    }
    for (int i = 0 ; i < num_tokens ; i++){
        free(tokens[i]);
    }
    free(tokens);
}
