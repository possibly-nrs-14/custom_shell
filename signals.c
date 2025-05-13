#include <stdio.h>
#include <signal.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include "all_functions.h"

void send_signal(pid_t pid, int signal){
    signal = signal % 32;
    if(!strncmp("Unknown", strsignal(signal), 7 )){
        print_red("invalid signal");
        return;
    }
    if (kill(pid, 0) == -1) {
        print_red("No such process found\n");
    }
    if (kill(pid, signal) == 0) {
        printf("Signal %d sent successfully to process %d\n", signal, pid);
    } 
}
void interrupt(int sig){
    printf("Signal SIGINT sent successfully to all fg processes\n");

}
void stop(int sig){
    for (int i = 0; i < fg_proc_count; i++) {
        kill(fg_proc_list[i].pid, SIGSTOP); 
    }
    printf("Signal SIGSTOP sent successfully to all fg processes\n");
    signal(SIGSTOP, SIG_DFL);
}
void kill_all(){
    for (int i = 0; i < proc_count; i++) {
        if (kill(proc_list[i].pid, SIGKILL) == -1){
            print_red("Failed to kill all processes\n");
        }
    }
    printf("Signal SIGKILL sent successfully to all processes\n");
}


void ping(char* command){
    int num_tokens = token_count(command);
    char** tokens = input_to_tokens(command, num_tokens);
    if (num_tokens < 3){
        print_red("invalid input\n");
        for (int i = 0 ; i < num_tokens ; i++){
            free(tokens[i]);
        }
        free(tokens);
        return;
    }
    send_signal(atoi(tokens[1]), atoi(tokens[2]));
    for (int i = 0 ; i < num_tokens ; i++){
        free(tokens[i]);
    }
    free(tokens);
}
