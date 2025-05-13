#include <stdio.h>
#include <unistd.h>
#include <string.h>
#include <stdlib.h>
#include <sys/wait.h>
#include "limits.h"
#include "all_functions.h"

int find_num_pipes(char* command){
    if (command[0] == '|' || command[strlen(command) - 1] == '|'){
        return 0;
    }
    int pipe_count = 0;
    for (int i = 0; i < strlen(command); i++){
        if (command[i] == '|'){
            pipe_count++;
        }
    }

    return pipe_count;
}



void pipe_handler(char* original_command, char* prev_dir, Log* log) {
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);
    
    // Create a copy of the command to avoid modification
    char* command = strdup(original_command);
    
    int pipe_count = find_num_pipes(command);
    if (pipe_count == 0) {
        print_red("invalid use of pipe");
        free(command);
        return;
    }
    
    char** sub_commands = malloc(sizeof(char*) * (pipe_count + 1));
    for (int i = 0; i < pipe_count + 1; i++) {
        sub_commands[i] = calloc(150, 1);
    }

    // Tokenize commands safely
    char* token = strtok(command, "|");
    int ind = 0;
    while (token != NULL) {
        token = remove_spaces_and_tabs_before_str(token);
        strcpy(sub_commands[ind], token);
        ind++;
        token = strtok(NULL, "|");
    }
    
    // Pipe handling
    int fd_in = STDIN_FILENO; 
    int pipe_fd[2];
    pid_t pid;
    
    for (int i = 0; i < pipe_count + 1; i++) {
        if (i < pipe_count) {
            if (pipe(pipe_fd) == -1) {
                print_red("pipe failed");
                goto cleanup;
            }
        }
        
        pid = fork();
        if (pid == 0) {  // Child process
            // Set up input redirection for this stage
            if (i > 0) {
                if (dup2(fd_in, STDIN_FILENO) < 0) {
                    perror("dup2 input failed");
                    exit(1);
                }
            }
            
            // Set up output redirection for this stage
            if (i < pipe_count) {
                if (dup2(pipe_fd[1], STDOUT_FILENO) < 0) {
                    perror("dup2 output failed");
                    exit(1);
                }
            }
            
            // Close unused pipe file descriptors
            if (i > 0) close(fd_in);
            if (i < pipe_count) close(pipe_fd[0]);
            close(pipe_fd[1]);
            
            // Execute command with I/O redirection
            if (strchr(sub_commands[i], '>') || strchr(sub_commands[i], '<')) {
                process_io_command(sub_commands[i], prev_dir, log, 1);
            } else {
                execute_command(sub_commands[i], prev_dir, log);
            }
            exit(0);
        } else if (pid < 0) {
            print_red("fork failed");
            goto cleanup;
        }
        
        // Parent process management
        if (i > 0) close(fd_in);
        if (i < pipe_count) {
            close(pipe_fd[1]);
            fd_in = pipe_fd[0];
        }
    }
    
    // Wait for all child processes
    while (wait(NULL) > 0);

cleanup:
    // Restore original standard I/O
    dup2(saved_stdin, STDIN_FILENO);
    dup2(saved_stdout, STDOUT_FILENO);
    
    // Free resources
    for (int i = 0; i < pipe_count + 1; i++) {
        free(sub_commands[i]);
    }
    free(sub_commands);
    free(command);
    
    close(saved_stdin);
    close(saved_stdout);
}
// int main(void){
//     // print_fd_target("stdin", STDIN_FILENO);
//     // print_fd_target("stdout", STDOUT_FILENO);
//     char* command = calloc(100, 1);
//     strcpy(command, "cat < log.c | wc | cat > b.txt");
//     pipe_handler(command, NULL, NULL);
//     free(command);

// }