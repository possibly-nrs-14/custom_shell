#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "all_functions.h"
Process fg_proc_list[100];
int fg_proc_count = 0;
Process proc_list[100];
int proc_count = 0;

void add_to_proc_list(char* cname, pid_t f){
    proc_list[proc_count].pid = f;
    strcpy(proc_list[proc_count++].command, cname);
}
void add_to_fg_proc_list(char* cname, pid_t f){
    fg_proc_list[fg_proc_count].pid = f;
    strcpy(fg_proc_list[fg_proc_count++].command, cname);
}
int compare_by_pid(const void *a, const void *b) {
    pid_t pid_a = ((Process *)a)->pid;
    pid_t pid_b = ((Process *)b)->pid;
    if (pid_a < pid_b) {
        return -1;
    } 
    else if (pid_a > pid_b) {
        return 1;
    } 
    else {
        return 0;
    }
}
void get_proc_info(pid_t pid) {
    char path[4096];
    sprintf(path, "/proc/%d/status", pid);  
    
    FILE* fp = fopen(path, "r");
    if (!fp) {
        print_red("error in opening file");
        return;
    }

    char line[4096];
    char state = ' ';
    char command[4096];
    
    while (fgets(line, sizeof(line), fp)) {
        if (strncmp(line, "Name:", 5) == 0) {
            sscanf(line, "Name: %s", command);  
        }
        if (strncmp(line, "State:", 6) == 0) {
            sscanf(line, "State: %c", &state);  
        }
    }
    fclose(fp);
    char* state_desc = (state == 'R' || state == 'S') ? "running" : "stopped";
    printf("%d: %s - %s\n", pid, command, state_desc);
}

void activities_handler(){
    qsort(proc_list, proc_count, sizeof(Process), compare_by_pid);
    for (int i = 0; i < proc_count; i++){
        get_proc_info(proc_list[i].pid);
    }
}

