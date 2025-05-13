#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/wait.h>
#include <signal.h>
#include <time.h>
#include "all_functions.h"

int execute_process(char* command, int background){
    
    char* home_dir = "/mnt/c/Users/Nikhil/Desktop/OSN/MP1";
    int num_tokens = token_count(command);
    char** tokens = input_to_tokens(command, num_tokens);
    int num_non_empty_strings = find_num_non_empty_strings(tokens, num_tokens);
    char** final_tokens = remove_empty_strings(tokens, num_tokens, num_non_empty_strings);
    // printf("%d\n", num_non_empty_strings);
    // char* command_ [] = {final_tokens[0], final_tokens[1], NULL};
    char** command_ = malloc((num_non_empty_strings + 1)* sizeof(char*));
    for (int i = 0; i < num_non_empty_strings; i++){
        command_[i] = final_tokens[i];
    }
    command_[num_non_empty_strings] = NULL;
    // printf("%d\n", num_non_empty_strings);
    // printf("commands_[1]: %s\n", command_[1]);
    // for (int i = 0; i < num_non_empty_strings + 1; i++){
    //     printf("%s\n", command_[i]);
    // }
    // exit(0);
  
        // final_tokens = command_;
        // final_tokens[num_non_empty_strings] = NULL;
        // printf("%d\n", num_non_empty_strings);
        // for (int i = 0; i < num_non_empty_strings; i++){
        //     printf("%s\n", command_[i]);
        // }
    // print_fd_target("stdin", STDIN_FILENO);  
    // print_fd_target("stdout", STDOUT_FILENO);
    int f = fork();
    if (f < 0){
        fprintf(stderr, "fork failed\n");
        exit(0);
    }
    else if (f == 0){
        
       
        if (execvp(command_[0], command_) == -1) {
            print_red("invalid command");
            _exit(EXIT_FAILURE); 
        
        }
    }
    else{
        add_to_proc_list(command_[0], f);
        // printf("ha\n");
        // printf("%d\n", background);
        if (background) {
            // printf("haa\n");
            // Background process: print the PID and return immediately
            printf("%d\n", f);
            return 1;
        }
        else{ 
            add_to_fg_proc_list(command_[0], f);
            // printf("hoi\n");
            time_t start = time(NULL);
            int status;
            waitpid(f, &status, 0);
            time_t end = time(NULL);

            if (difftime(end, start) > 2) {
                char* time_info = malloc(50); 
                sprintf(time_info, " %s : %lds",command, (long)difftime(end, start));  
                char*hostname = get_sysname();
                uid_t uid = getuid();
                char* username = get_username(uid);
                char* prompt = create_prompt(username, hostname, 1, time_info);
                printf("%s ", prompt);
                return 1;     
            }
        }
    }
    
    
    for (int i = 0; i < num_non_empty_strings; i++){
        free(final_tokens[i]);
    }
    free(final_tokens);
    free(command_);
    
    return 0;
}

void handle_sigchild(int sig) {
    int status;
    pid_t pid;

    // Wait for all child processes that have terminated
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        if (WIFEXITED(status)) {
            printf("\nProcess %d exited normally with status %d\n", pid, WEXITSTATUS(status));
        } 
        else if (WIFSIGNALED(status)) {
            printf("\nProcess %d was terminated by signal %d\n", pid, WTERMSIG(status));
        }
    }
    fflush(stdout);
}
// int main(void){
//     signal(SIGCHLD, handle_sigchild);
//     char input[4096];
//     while(1){
//         if (!fgets(input, sizeof(input), stdin)) {
//             printf("Error reading input.\n");
//             exit(0);
//         }
//         size_t len = strlen(input);
//         if (len > 0 && input[len - 1] == '\n') {
//             input[len - 1] = '\0';
//         }

//         int background = 0;
//         if (input[len - 2] == '&') {
//             background = 1;
//             input[len - 2] = '\0'; // Remove '&' from command
//         }
//         execute_process(input, background);
//     }
// }

