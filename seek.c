# define _XOPEN_SOURCE 500
# define __USE_ATFILE
# define _DEFAULT_SOURCE
# define __USE_FILE_OFFSET64
# define _GNU_SOURCE
#include <ftw.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <limits.h>
#include "all_functions.h"
#define MAX_FILES 1000  // Maximum number of files to store
char **file_paths; 
int file_count = 0;

int get_file_paths(const char *fpath, const struct stat *sb, int typeflag, struct FTW *ftwbuf) {
    file_paths[file_count] = calloc(strlen(fpath) + 1, 1);
    strcpy(file_paths[file_count], fpath);
    file_paths[file_count][strlen(fpath)] = '\0';
    file_count++;
    return 0;  
}

int is_prefix(char* str1, char* str2){  //checks if str1 is a prefix of str2
    if (strlen(str1) > strlen(str2)){
        return 0;
    }
    for (int i = 0; i < strlen(str1); i++){
        if (str1[i] != str2[i]){
            return 0;
        }
    }
    return 1;
}
int find_num_fwd_slashes(char* path){
    if (strlen(path) == 0){
        return 0;
    }
    int num_slashes = 0;
    for (int i = 0; i < strlen(path); i++){
        // printf("huh\n");
        if (path[i] == '/'){
            num_slashes++;
        }
    }
    return num_slashes;
}
char* get_name_from_path(char* path){
    if (strlen(path) == 0){
        return 0;
    }
    char* name = calloc(100, 1);
    int ind = 0;
    int num_slashes = find_num_fwd_slashes(path);
    for (int i = 0; i < strlen(path); i++){
        if (path[i] == '/'){
            num_slashes--;
        }
        if (num_slashes == 0 && path[i] != '/'){
            name[ind++] = path[i];
        }
    }
    return name;
}
void print_file_contents(char *filename) {
    FILE *file = fopen(filename, "rb");  
    if (file == NULL) {
        print_red("error opening file");
        return; 
    }

    unsigned char buffer[1024];
    size_t bytesRead;

    while ((bytesRead = fread(buffer, 1, sizeof(buffer), file)) > 0) {
        fwrite(buffer, 1, bytesRead, stdout);  
    }
    if (ferror(file)) {
       print_red("error reading file");
       return;
    }
    printf("\n");
    fclose(file);
}
int has_perms(char* path, int isdir){
    struct stat st;
    if (stat(path, &st) == 0) {
        char permissions[5];  
        sprintf(permissions, "%o", st.st_mode & 0777);
        int p = permissions[2] - '0';
        if(!isdir){
            if (p >= 4){
                return 1;
            }
        }
        else{
            if (p == 7 || p == 5 || p == 3 || p == 1){
                return 1;
            }
        }

    } 
    else {
        print_red("stat failed");
        return 0;
    }
    return 0;
}
void seek_controller(char* command, char* prev_dir){
    char* home_dir = "/mnt/c/Users/Nikhil/Desktop/OSN/MP1";
    int num_tokens = token_count(command);
    if (num_tokens < 2){
        print_red("Invalid command");
        return;
    }
    char** tokens = input_to_tokens(command, num_tokens);
    int d_flag = 0;
    int f_flag = 0;
    int e_flag = 0; 
    char* target = calloc(4097, 1);
    char* target_dir = calloc(4097, 1);
    for (int i = 1; i < num_tokens; i++){
        if (tokens[i][0] == '-'){
            if (!strcmp(tokens[i], "-")){
                if (num_tokens == 2){
                    free(target);
                    free(target_dir);
                    print_red("Invalid command");
                    return;
                }
                else{
                    if (i == num_tokens - 1){
                        if (strlen(prev_dir) > 0){
                            strcpy(target_dir, prev_dir);
                        }
                        strcpy(target, tokens[i - 1]);
                    }
                    else{
                        free(target);
                        free(target_dir);
                        print_red("Invalid command");
                        return;

                    }
                }
            }
            else if (strlen(tokens[i]) == 2){
                if (tokens[i][1] == 'd'){
                    d_flag = 1;
                }
                if (tokens[i][1] == 'f'){
                    f_flag = 1;
                }
                if (tokens[i][1] == 'e'){
                    e_flag = 1;
                }
                if (d_flag && f_flag){
                    free(target);
                    free(target_dir);
                    for (int i = 0; i < file_count; i++){
                        free(file_paths[i]);
                    }
                    free(file_paths);
                    print_red("Invalid flags");
                    return;
                }

            }
            else{
                free(target);
                free(target_dir);
                print_red("Invalid flags");
                return;
            }
        }
        else if(tokens[i][0] == '~'){
            if (num_tokens == 2){
                print_red("Invalid command");
                free(target);
                free(target_dir);
                return;
            }
            if (!strcmp(tokens[i], "~")){
                strcpy(target_dir, home_dir);
                if (i == num_tokens - 1){
                    if (tokens[i - 1][0] == '-'){
                        print_red("Invalid command");
                        free(target);
                        free(target_dir);
                        return;
                    }
                    else{
                        strcpy(target, tokens[i - 1]);
                    }
                }
                
                else{
                    print_red("Invalid command");
                    free(target);
                    free(target_dir);
                    return;
                }
            }
            else{
                char* dir = calloc(PATH_MAX, 1);
                slice(tokens[i], dir, 2, strlen(tokens[i]));
                strcpy(target_dir, dir);
                free(dir);
                if (i == num_tokens - 1){
                    if (tokens[i - 1][0] == '-'){
                        print_red("Invalid command");
                        free(target);
                        free(target_dir);
                        return;
                    }
                    else{
                        strcpy(target, tokens[i - 1]);
                    }
                }
                
                else{
                    print_red("Invalid command");
                    free(target);
                    free(target_dir);
                    return;
                }
                
            }
            
        }
        else{
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                free(target);
                free(target_dir);
                print_red("getcwd error");
                return;
            }
            if (num_tokens == 2){
                strcpy(target, tokens[1]);
                strcpy(target_dir, cwd);
            }
            else if (i == num_tokens - 1){
                if (tokens[i - 1][0] == '-' && strcmp(tokens[i - 1], "-")){
                    strcpy(target_dir, cwd);
                    strcpy(target, tokens[i]);
                }
                else if (!strcmp(tokens[i - 1], "-") || !strcmp(tokens[i - 1], "~")){
                    print_red("Invalid command");
                    free(target);
                    free(target_dir);
                    return;
                }
                else{
                    strcpy(target, tokens[i - 1]);
                    strcpy(target_dir, tokens[i]);
                }

            }
               
        }
    }
    if (strlen(target) == 0 || strlen(target_dir) == 0 || !is_dir(target_dir)){
        free(target);
        free(target_dir);
        print_red("Invalid command");
        return;
    }
    file_paths = calloc(sizeof(char*) * MAX_FILES, 1);
    if (nftw(target_dir, get_file_paths, 20, 0) == -1) {
        print_red("error in traversing directory");
    }
    // for (int i = 0; i < file_count; i++){
    //     print_color_coded_name(file_paths[i], name);
    // }
    if (file_count == 0){
        free(file_paths);
        print_red("empty directory");
        return;
    }
    if (!d_flag && !f_flag){
        int flag = 0;
        int first = 0;
        for (int i = 0; i < file_count; i++){
            char* name = get_name_from_path(file_paths[i]);
            if (is_prefix(target, name)){
                first = i;
                flag++;
                print_color_coded_name(file_paths[i], name, 1);
            }
            free(name);
        }
        if (!flag){
            print_red("no match found");
        }
        else if (e_flag){
            if (flag == 1){
                if (!has_perms(file_paths[first], 0) && !has_perms(file_paths[first], 1)){
                    print_red("missing permissions for task");
                }
                else if(is_dir(file_paths[first])){
                    if (chdir(file_paths[first]) != 0){
                        print_red("error in changing directory");
                    }
                }
                else if(!is_dir(file_paths[first])){
                    print_file_contents(file_paths[first]);
                }
            }
            
        }
    }
    else if (d_flag){
        int flag = 0;
        int first = 0;
        for (int i = 0; i < file_count; i++){
            char* name = get_name_from_path(file_paths[i]);
            if (is_prefix(target, name) && is_dir(file_paths[i])){
                first = i;
                flag++;
                print_color_coded_name(file_paths[i], name, 1);
            }
            free(name);
        }
        if (!flag){
            print_red("no match found");
        }
        else if (e_flag){
            if (flag == 1){
                if (!has_perms(file_paths[first], 1)){
                    print_red("missing permissions for task");
                }
                else if(is_dir(file_paths[first])){
                    if (chdir(file_paths[first]) != 0){
                        print_red("error in changing directory");
                    }
                }
            }
        }
    }
    else if (f_flag){
        int flag = 0;
        int first = 0;
        for (int i = 0; i < file_count; i++){
            char* name = get_name_from_path(file_paths[i]);
            if (is_prefix(target, name) && !is_dir(file_paths[i])){
                first = i;
                flag++;
                print_color_coded_name(file_paths[i], name, 1);
            }
            free(name);
        }
        if (!flag){
            print_red("no match found");
        }
        else if (e_flag){
            if (flag == 1){
                if (!has_perms(file_paths[first], 0)){
                    print_red("missing permissions for task");
                }
                else if(!is_dir(file_paths[first])){
                    print_file_contents(file_paths[first]);
                }
            }
        }


    }
    free(target);
    free(target_dir);
    for (int i = 0; i < file_count; i++){
        free(file_paths[i]);
    }
    free(file_paths);
    for(int i = 0; i < num_tokens; i++){
        free(tokens[i]);
    }
    free(tokens);
    file_count = 0;
}
// int main(void) {
//     char* input = calloc(4096, 1);
//     if (fgets(input, 4096, stdin) == NULL) {
//         free(input);
//     }
//     input[strcspn(input, "\n")] = 0;
//     seek_controller(input, "./rabbit");
//     free(input);

// }
