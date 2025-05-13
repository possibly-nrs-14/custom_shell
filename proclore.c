#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include "all_functions.h"
void get_process_info(pid_t pid) {
    char path[256];
    char buffer[1024];
    FILE *fp;

    // 1. Process Status
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    fp = fopen(path, "r");
    if (fp == NULL) {
        print_red("failed to open file");
        return;
    }

    // The 3rd token is the process status
    char* state = malloc(10);
    while (fgets(buffer, sizeof(buffer), fp)) {
        // printf("%s\n", buffer);
        if (strncmp(buffer, "State:", 6) == 0) {
            sscanf(buffer, "State: %s", state);
            // printf("%s\n", state);
            break;
        }
    }
    fclose(fp);

    // 3. Virtual Memory
    snprintf(path, sizeof(path), "/proc/%d/status", pid);
    fp = fopen(path, "r");
    if (fp == NULL) {
        perror("fopen");
        return;
    }
    size_t vmsize = 0;
    while (fgets(buffer, sizeof(buffer), fp)) {
        // printf("%s\n", buffer);
        if (strncmp(buffer, "VmSize:", 7) == 0) {
            sscanf(buffer, "VmSize: %lu kB", &vmsize);
            break;
        }
    }

    fclose(fp);

    // 4. Executable Path
    snprintf(path, sizeof(path), "/proc/%d/exe", pid);
    ssize_t len = readlink(path, buffer, sizeof(buffer) - 1);
    if (len != -1) {
        buffer[len] = '\0';
    } 
    else {
        strcpy(buffer, "access to this path is prohibited");
    }

    // Print the information
    printf("PID: %d\n", pid);
    printf("Process Status: %s\n", state);
    printf("Process Group: %d\n", getpgid(pid));
    printf("Virtual Memory: %lu kB\n", vmsize);
    printf("Executable Path: %s\n", buffer);
    free(state);
}
void proclore_controller(char* command){
    // printf("lol\n");
    int num_tokens = token_count(command);
    // printf("%d\n", num_tokens);
    char** tokens = input_to_tokens(command, num_tokens);
    if (num_tokens == 1){
        get_process_info(1);

    }
    else if(num_tokens == 2){    
    int p_id = atoi(tokens[1]);
    get_process_info(p_id);
    }
    else{
        print_red("invalid input");
    }
    for (int i = 0; i < num_tokens; i++){
        free(tokens[i]);
    }
    free(tokens);
}

// int main(void) {
//     // get_process_info(31339);
//     char* command = malloc(40);
//     command = "proclore 31339";
//     proclore_controller(command);
//     free(command);
   
//     // return 0;
// }
