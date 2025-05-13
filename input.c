#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include <limits.h>
#include "all_functions.h"
int token_count(char* str){
    int num_tokens = 0;
    for (int i = 0; i < strlen(str); i++){
            if (str[i] == ' '){
                // printf("space at index: %d\n", i);
                num_tokens++;
            }       
        }
    num_tokens++;
    return num_tokens;
}
char* remove_extra_spaces_and_tabs(char* input){
    int ind = 0; 
    char* inp = calloc(4096, 1);
    int q_flag = 0; 
    int space_count = 0;
    for (int i = 0; i < strlen(input); i++){
        if (input[i] != ' ' && input[i] != '\t'){
            if (input[i] == '"'){
                q_flag = !q_flag;
            }
            inp[ind++] = input[i];
            space_count = 0;
        }
        else{ 
            if(input[i] == ' '){
                space_count++;
                if (space_count == 1 || q_flag == 1){
                    inp[ind++] = input[i];
                } 
            }
            else if (input[i] == '\t'){
               if (q_flag == 1){
                inp[ind++] = input[i];
               } 
            }
        }
    }
    while (ind > 0 && (inp[ind - 1] == ' ' || inp[ind - 1] == '\t')) {
        ind--;
    }
    inp[ind] = '\0';
    return inp;
}
char* remove_spaces_and_tabs_before_str(char* str){
    while (*str == ' ' || *str == '\t'){
        str++;
    }
    return str;
}
char** input_to_tokens(char* input, int num_tokens){
    // printf("%s\n", input);
    char** tokens = malloc(sizeof(char*) * num_tokens);
    for (int i = 0; i < num_tokens; i++){
        tokens[i] = calloc(150, 1);
    }                                                                                                                                                                                                                                                                                                                                                                                                                                                                   
    char* token = strtok(input, " ");
    int ind = 0;
    while (token != NULL) {
        strcpy(tokens[ind], token);
        ind++;
        token = strtok(NULL, " ");
    }
    
    int q_flag = 0;
    int start = 0;
    for (int i = 0; i < num_tokens; i++){
        if (tokens[i][0] == '"'){
            q_flag = 1;
            start = i;
        }
        if (q_flag){
            if (i != start){
                strcat(tokens[start], " ");
                strcat(tokens[start], tokens[i]);
                tokens[i] = "";
            }
        }
        if (strlen(tokens[i]) > 0){
            if (tokens[i][strlen(tokens[i]) - 1] == '"'){
                q_flag = 0;
            }
        }
        
    }
    // for (int i = 0; i < num_tokens; i++){
    //     printf("%s\n", tokens[i]);
    // }
    return tokens;
}
char** parse_input(char* input, int num_tokens){
    char** commands = input_to_tokens(input, num_tokens);
    // printf("commands before & processing:\n");
    // for (int i = 0; i < num_tokens; i++){
    //     printf("%s\n", commands[i]);
    // }
    int start = 0;
    for (int i = 0; i < num_tokens; i++){
        if (!strcmp(commands[i], ";")){
            commands[i] = "";
            if(start < num_tokens - 1){
                start = i + 1;
            }
        }
        else if(!strcmp(commands[i], "&")){
            // printf("after\n");
            commands[i][strlen(commands[i]) - 1] = '\0';
            strcat(commands[start], " &");
            start = i + 1;
        }
        else{
            if (i != start){
                strcat(commands[start], " ");
                strcat(commands[start], commands[i]);
                commands[i] = "";
            }
        }
    }
    // printf("printing commands after & processing:\n");
    return commands;
}
int find_num_non_empty_strings(char** commands, int num_tokens){
    int num_non_empty_strings = 0;
    for (int i = 0; i < num_tokens; i++){
        if (strcmp(commands[i], "")){
            num_non_empty_strings++;
        }
    }
    return num_non_empty_strings;
}
char** remove_empty_strings(char** commands, int num_tokens, int num_non_empty_strings) {
    char **final_commands = malloc(sizeof(char *) * (num_non_empty_strings + 1)); 
    if (!final_commands) {
        perror("malloc failed");
        exit(EXIT_FAILURE);
    }

    int ind = 0;
    for (int i = 0; i < num_tokens; i++) {
        if (strcmp(commands[i], "")) { 
            size_t len = strlen(commands[i]) + 1; 
            final_commands[ind] = malloc(len);
            if (!final_commands[ind]) {
                perror("malloc failed");
                exit(EXIT_FAILURE);
            }
            strcpy(final_commands[ind], commands[i]);
            // free(commands[i]);
            ind++;
        }
    }
    final_commands[ind] = NULL; 

    return final_commands;
}

// int main(void){
//     char input[4096];
//     int num_tokens = 0;
//     if (!fgets(input, sizeof(input), stdin)) {
//         printf("Error reading input.\n");
//         exit(0);
//     }
//     else{
//         size_t len = strlen(input);
//         if (len > 0 && input[len - 1] == '\n') {
//             input[len - 1] = '\0';
//         }
//     }
//     char* inp = remove_extra_spaces_and_tabs(input);
//     num_tokens = token_count(inp);
//     // printf("%s\n", inp);
//     char** commands = parse_input(inp, num_tokens);
//     int num_non_empty_strings = find_num_non_empty_strings(commands, num_tokens);
//     char** final_commands = remove_empty_strings(commands, num_tokens, num_non_empty_strings);
//     for (int i = 0; i < num_non_empty_strings; i++){
//         printf("%s\n", final_commands[i]);
//     }
// }