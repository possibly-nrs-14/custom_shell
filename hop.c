#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include <limits.h>
#include "all_functions.h"
char* hop(char* next_dir){
    struct stat sb;
    if (!stat(next_dir, &sb) == 0 || !S_ISDIR(sb.st_mode)) {
        printf("Directory you want to go to doesn't exist\n");
        return NULL;
    } 
    if (chdir(next_dir) == 0){
        char buff[PATH_MAX];
        if (!getcwd(buff, sizeof(buff))){
            printf("error in getting directory\n");
            return NULL;
        }

        char* dir = calloc(PATH_MAX, 1);
        strcpy(dir, buff);
        return dir;
    }
    else{
        printf("error in changing directory\n");
        return NULL;
    }
}
void hop_controller(char* command, char* prev_dir){
    char* home_dir = "/mnt/c/Users/Nikhil/Desktop/OSN/MP1";
    int args_len = token_count(command);
    int a = args_len;
    char** args = input_to_tokens(command, args_len);
    int num_non_empty_strings = find_num_non_empty_strings(args, args_len);
    char** final_args = remove_empty_strings(args, args_len, num_non_empty_strings);
    if (strcmp(final_args[0], "hop")){
        printf("invalid input\n");
        return;
    }
    args_len = num_non_empty_strings;
    // for (int i = 0; i < args_len; i++){
    //     printf("%s\n", final_args[i]);
    // }
    // char* prev_dir = malloc(PATH_MAX);
    if (!final_args){
        print_red("empty command");
    }
    else if (args_len == 1){
        // printf("ay\n");
        if (!strcmp(final_args[0], "hop")){
            char buff[PATH_MAX];
            if (!getcwd(buff, sizeof(buff))){
                print_red("error in getting directory");
            }
            strcpy(prev_dir, buff);
            char* next_dir = hop(home_dir);
            printf("%s\n", next_dir);
            free(next_dir);
        }
    }
    else{
        // printf("entered\n");
        for (int i = 1; i < args_len; i++){
            if (!strcmp(final_args[i], "~")){
                // printf("q\n");
                char buff[PATH_MAX];
                if (!getcwd(buff, sizeof(buff))){
                    print_red("error in getting directory");
                }
                strcpy(prev_dir, buff);
                char* next_dir = hop(home_dir);
                if (next_dir){
                    printf("%s\n", next_dir);
                    free(next_dir);
                }
            }
            else if (!strcmp(final_args[i], "-")){
                // printf("w\n");
                if (strlen(prev_dir) > 0){
                    char* curr_dir = calloc(PATH_MAX, 1);
                    char buff[PATH_MAX];
                    if (!getcwd(buff, sizeof(buff))){
                        print_red("error in getting directory");
                    }
                    strcpy(curr_dir, buff);
                    char* next_dir = hop(prev_dir);
                    strcpy(prev_dir, curr_dir);
                    free(curr_dir);
                    if (next_dir){
                        printf("%s\n", next_dir);
                        free(next_dir);
                    }
                }
                else{
                    print_red("oldpwd not set");
                }
            }
            else{
                // printf("e\n");
                if (is_substring("~", final_args[i])){
                    char* dir = calloc(PATH_MAX, 1);
                    slice(final_args[i], dir, 2, strlen(final_args[i]));
                    char buff[PATH_MAX];
                    if (!getcwd(buff, sizeof(buff))){
                        print_red("error in getting directory");
                    }
                    strcpy(prev_dir, buff);
                    char* next_dir = hop(dir);
                    if (next_dir){
                        printf("%s\n", next_dir);
                        free(next_dir);
                    }
                    free(dir);
                }
                else{
                    
                    char buff[PATH_MAX];
                    
                    if (!getcwd(buff, sizeof(buff))){
                        print_red("error in getting directory\n");
                    }
                    // printf("Ff\n");
                    strcpy(prev_dir, buff);
                    char* next_dir = hop(final_args[i]);
                    if (next_dir){
                        printf("%s\n", next_dir);
                        free(next_dir);
                    }
                }
            }
        }
    }
    for (int i = 0; i < args_len; i++){
        free(final_args[i]);
    }
    free(final_args);
    
    
    // free(last_dir);       
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
//         hop_controller(final_commands[i]);
//     }
//     for (int i = 0; i < num_non_empty_strings; i++){
//         free(final_commands[i]);
//     }
//     free(final_commands);
//     free(inp);
// }