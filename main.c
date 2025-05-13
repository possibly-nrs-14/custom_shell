#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <signal.h>
#include "all_functions.h"
#define CONFIG_FILE ".my_shrc"
#define MAX_COMMANDS_IN_FUNCTION 20
char* prev_dir;
int num_aliases = 0;
int cont = 1;
int add_time_to_prompt = 0;
void add_new_alias(char *line) {
    char *name = strtok(line + 6, "="); 
    name[strlen(name) - 1] = '\0';
    char *command = strtok(line + 6 + strlen(name) + 4 , "'");   
    if (name && command) {
        strcpy(alias_list[num_aliases].name, name);
        strcpy(alias_list[num_aliases++].command, command);
    }
}

char *replace_alias(char *command) {
    for (int i = 0; i < num_aliases; i++) {
        if (!strcmp(command, alias_list[i].name)) {
            return alias_list[i].command;
        }
    }
    return command; 
}

int execute_command(char *command, char *prev_dir, Log* log) {
    if (strchr(command, '|')){
        pipe_handler(command, prev_dir, log);
        
    }
    else if (strchr(command, '>') || strchr(command, '<')){
        process_io_command(command, prev_dir, log, 0);
    }
    else if (!strncmp("hop", command, 3)) {
        if (is_substring ("&", command)){
            print_red("invalid command");
            return 1;
        }
        hop_controller(command, prev_dir); 
    } 
    else if (!strncmp("reveal", command, 6)) {
        printf("%s\n", command);
        if (is_substring ("&", command)){
            print_red("invalid command");
            return 1;
        }
        reveal(command, prev_dir); 
        total_blocks = 0;
    } 
    else if (!strncmp("log", command, 3)){
        if (is_substring ("&", command)){
            print_red("invalid command");
            return 1;
        }
        int l = log_controller(command, log, prev_dir);
        if (!l){
            return 0;
        }
        else if (l == 2){
            add_time_to_prompt = 1;
        }
    }
    else if (!strncmp("proclore", command, 8)){
        if (is_substring ("&", command)){
            print_red("invalid command");
            return 1;
        }
        proclore_controller(command);
    }
    else if (!strncmp("seek", command, 4)){
        if (is_substring ("&", command)){
            print_red("invalid command");
            return 1;
        }
        seek_controller(command, prev_dir);
    }
    else if (!strcmp("end", command)){
        return 0;
    }
    
    else if (!strcmp("activities", command)){
        activities_handler();
    }
    else if (!strncmp("ping", command, 4)){
        ping(command);
    }
    else if (!strncmp("fg", command, 2) || !strncmp("bg", command, 2)){
        fg_bg_handler(command);
    }
    else if (!strncmp("iMan", command, 4)){
        iMan_handler(command);
    }
    else {
        size_t len = strlen(command);
        int background = 0;
        if (is_substring("&", command)) {
            background = 1;
            for (int i = 0; i < len; i++){
                if (command[i] == '&'){
                    command[i] = '\0';
                }  
            }
        }
        // printf("%s\n", command);
        add_time_to_prompt = execute_process(command, background);
    }
    return 1;
}

void load_config_file() {
    char * config_file_path = "/mnt/c/Users/Nikhil/Desktop/OSN/MP1/.myshrc";
    FILE *file = fopen(config_file_path, "r");
    if (!file) {
        printf("Failed to open configuration file\n");
        return;
    }
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        line[strcspn(line, "\n")] = 0; 
        if (!strncmp(line, "alias", 5)) {
            add_new_alias(line);
        } 
    }

    fclose(file);
}

int main(void) {
    signal(SIGCHLD, handle_sigchild);
    signal(SIGINT, interrupt);
    signal(SIGTSTP, stop);
    load_config_file();
    char *prompt;
    prev_dir = calloc(4096, 1); 
    char * home_dir = "/mnt/c/Users/Nikhil/Desktop/OSN/MP1";
    Log *log = init_log();
    load_log_from_file(log);
    // Main shell loop
    while (cont) {
        // Create and display the prompt
        char* input = malloc(4096);
        char* hostname = get_sysname();
        uid_t uid = getuid();
        char* username = get_username(uid);
        if(!add_time_to_prompt){
            prompt = create_prompt(username, hostname, 0, NULL);
            printf("%s ", prompt);
            free(prompt);
        }
        add_time_to_prompt = 0;
        free(hostname);
        if (fgets(input, 4096, stdin) == NULL) {
            kill_all();
            free(input);
            break; 
        }
         
        input[strcspn(input, "\n")] = 0;
        size_t len = strlen(input);
        if (len == 0) {
            free(input);
            continue; 
        }
        char* command = malloc(4096);
        strcpy(command, input);
        add_to_log(log, command);
        free(command);

        // Clean and tokenize the input
        char *cleaned_input = remove_extra_spaces_and_tabs(input);
        int num_tokens = token_count(cleaned_input);
        char **commands = parse_input(cleaned_input, num_tokens);
        int num_non_empty_strings = find_num_non_empty_strings(commands, num_tokens);
        char **final_commands = remove_empty_strings(commands, num_tokens, num_non_empty_strings);
        for (int i = 0; i < num_non_empty_strings; i++) {
            char*comm = replace_alias(final_commands[i]);
            cont = execute_command(comm, prev_dir, log);
            
            if (!cont){
                break;
            }
        }

        // Free memory
        free(cleaned_input);
        for (int i = 0; i < num_non_empty_strings; i++) {
            free(final_commands[i]);
        }
        free(final_commands);
        free(input);
        
    }
    save_log_to_file(log);
    free_linked_list(log->node);
    free(log);
    free(prev_dir);
}



