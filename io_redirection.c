#include <stdio.h>
#include <unistd.h>
#include <fcntl.h>
#include <stdlib.h>
#include <string.h>
#include <limits.h>
#include "all_functions.h"

char* find_substring_before_char(char* str, char c) {
    char* pos = strchr(str, c);
    if (!pos) {
        return NULL;  
    } 
    int length = pos - str;
    char* result = malloc(length + 1);  
    strncpy(result, str, length);
    result[length] = '\0'; 
    return result;
}

void process_io_command(char* command, char* prev_dir, Log* log, int p){
    int saved_stdin = dup(STDIN_FILENO);
    int saved_stdout = dup(STDOUT_FILENO);
    char* co = malloc(strlen(command));
    char* input_file = calloc(strlen(command), 1);
    char* output_file = calloc(strlen(command), 1);
    char* result1 = strchr(command, '>');
    int fd_in = 0;
    int fd_out = 0;
    if (result1){
        if (result1[1] != '>'){  // >
            int start = 1;
            for(int i = 1; i < strlen(result1); i++){
                if (result1[i] != ' '){
                    start = i;
                    break;
                }
            }
            // printf("%s  %d\n", result1, start);
            slice(result1, output_file, start, strlen(result1));
            // printf("%s   %ld\n", output_file, strlen(output_file));
            fd_out = open(output_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
            // printf("%d\n", fd_out);
            if (fd_out < 0){
                print_red("failed to open file");
                free(input_file);
                free(output_file);
                free(co);
                return;
            }
            

        }
        else{  // >>
            int start = 1;
            for(int i = 2; i < strlen(result1); i++){
                if (result1[i] != ' '){
                    start = i;
                    break;
                }
            }
            slice(result1, output_file, start, strlen(result1));
            fd_out = open(output_file, O_WRONLY | O_APPEND | O_CREAT, 0644);
            if (fd_out < 0){
                print_red("failed to open file");
                free(input_file);
                free(output_file);
                free(co);
                return;
            }
        
            
            
        }
    }
    char* result2 = strchr(command, '<');
    if(result2){
        int start = 1;
        int end = 1;
        for(int i = 1; i < strlen(result2); i++){
            if (result2[i] != ' '){
                start = i;
                break;
            }  
        }
        for(int i = start; i < strlen(result2); i++){
            if (result2[i] == ' ' || result2[i] == '>' ){
                end = i;
                break;
            }  
        }
        slice(result2, input_file, start, end == 1 ? strlen(result2) : end);
        fd_in = open(input_file, O_RDONLY);
        if (fd_in < 0){
            print_red("file to be read from doesn't exist");
            free(input_file);
            free(output_file);
            free(co);
            return;
        }
    }
    char* str = find_substring_before_char(command, '>');
    if (str){
        if (is_substring("<", str)){
            char*strr = find_substring_before_char(str, '<');
            strcpy(co, strr);
            free(strr);
        }
        else{
            str[strlen(str) - 1] = '\0';
            // printf("%s  %ld\n", str, strlen(str));
            strcpy(co, str);
        }
    }
    char* str2 = find_substring_before_char(command, '<');
    if (str2){
        if (is_substring(">", str2)){
            free(str);
            free(co);
            free(str2);
            free(input_file);
            free(output_file);
            return;
        }
        else{
            str2[strlen(str2) - 1] = '\0';
            strcpy(co, str2);
        }
    }
    if (strlen(input_file) > 0){
        if(dup2(fd_in, STDIN_FILENO) < 0){
            print_red("failed to redirect stdin");
            free(input_file);
            free(output_file);
            free(co);
            return;
        }
    }
    if (strlen(output_file) > 0){
        if(dup2(fd_out, STDOUT_FILENO) < 0){
            print_red("failed to redirect stdout");
            free(input_file);
            free(output_file);
            free(co);
            return;
        }
    }
    execute_command(co, prev_dir, log);
    if ((strlen(input_file)) > 0){
        close(fd_in);
    }
    if ((strlen(output_file)) > 0){
        close(fd_out);
    }
    if (!p){
        if(dup2(saved_stdin, STDIN_FILENO) < 0){
            print_red("error in redirecting io");
        }
        
            
        if(dup2(saved_stdout, STDOUT_FILENO) < 0){
            print_red("error in redirecting io");
        }
    }  
    close(saved_stdin);
    close(saved_stdout);
    free(str);
    free(co);
    free(str2);
    free(input_file);
    free(output_file);
    
}
// int main(void){
//     char* str = calloc(PATH_MAX, 1);
//     process_io_command("cat < a.txt >> b.txt",str,NULL);
//     free(str);
// }