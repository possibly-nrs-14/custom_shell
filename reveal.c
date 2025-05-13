#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <pwd.h>
#include <grp.h>
#include <time.h>
#include <fcntl.h>
#include <string.h> 
#include <limits.h>
#include <dirent.h>
#include "all_functions.h"
int total_blocks = 0;
int find_num_files(char* dir, int flag){
    DIR *d = opendir(dir);
    struct dirent *entry;
    if (d == NULL) {
        print_red("opendir error");
        return 0;
    }
    int num_files = 0;
    while ((entry = readdir(d)) != NULL) {
        if (!flag){
            if (entry->d_name[0] != '.'){
                num_files ++;
            }
        }
        else{
            num_files++;
        }
    }
    closedir(d);
    return num_files;
}

char** get_filenames(char* dir, int flag){
    DIR *d = opendir(dir);
    if (d == NULL) {
        print_red("opendir error");
        return NULL;
    }
    struct dirent *entry;
    struct stat file_stat;
    int num_files = find_num_files(dir, flag);
    char** filenames = malloc(sizeof(char*) * num_files);
    for (int i = 0; i < num_files; i++){
        filenames[i] = malloc(150);
    }
    int ind = 0;
    while ((entry = readdir(d)) != NULL) {
        if (!flag){
            if (entry->d_name[0] != '.'){
                // printf("%s\n", entry->d_name);
                strcpy(filenames[ind++], entry->d_name);
                total_blocks += file_stat.st_blocks;
            }
        }
        else{
           strcpy(filenames[ind++], entry->d_name);
           total_blocks += file_stat.st_blocks;
        }
    }
    closedir(d);
    return filenames;
}
int is_dir(char* obj){
    struct stat file_stat;
    
    // Get file stats
    if (stat(obj, &file_stat) == -1) {
        print_red("stat error");
        return 0;
    }
    return S_ISDIR(file_stat.st_mode);
}
int is_lexicographically_smaller_than(char* str1, char* str2){
        int p1 = 0;
        int p2 = 0;
        int l1 = strlen(str1);
        int l2 = strlen(str2);
        while (p1 < l1 && p2 < l2){
            if (str1[p1] < str2[p2]){
                return 1;
            }
            else if (str1[p1] > str2[p2]){
                return 0;
            }
            else{
                p1++;
                p2++;
            }
        }
        return l1 < l2 ? 1 : 0;
}
void merge_sort_filenames(char** filenames, int num_files){
    if (num_files == 1) {
        return;
    } 
    else {
        int midpt = num_files / 2;
        char** left = malloc(sizeof(char*) * midpt);
        char** right = malloc(sizeof(char*) * (num_files - midpt));
        for (int i = 0; i < midpt; i++){
            left[i] = malloc(250);
        }
        for (int i = 0; i < num_files - midpt; i++){
            right[i] = malloc(250);
        }

        for(int i = 0;i < midpt; i++){
            strcpy(left[i], filenames[i]);
        }
        for(int i = 0;i < num_files - midpt; i++){
            strcpy(right[i], filenames[i + midpt]);
        }
        merge_sort_filenames(left, midpt);
        merge_sort_filenames(right, num_files - midpt);
        int l_ind = 0, r_ind = 0;
        for(int i = 0;i < num_files ; i++){
           if(l_ind < midpt && r_ind < num_files - midpt){
              if(is_lexicographically_smaller_than(left[l_ind], right[r_ind])){
                strcpy(filenames[i], left[l_ind++]);
              } 
              else{
                strcpy(filenames[i], right[r_ind++]);
              }
           }
           else{
              if(l_ind >= midpt){
                strcpy(filenames[i], right[r_ind++]);
              }
              else if(r_ind >= num_files -midpt){
                strcpy(filenames[i], left[l_ind++]);
              }
           }
        }
        for (int i = 0; i < midpt; i++){
            free(left[i]);
        }
        for (int i = 0; i < num_files - midpt; i++){
            free(right[i]);
        }
        free(left);  
        free(right);

    }
}
char* get_actual_path(char* relative_path, char* path){
    // printf("relative path: %s\n", relative_path);
    // printf("path: %s\n", path);
    char full_path[PATH_MAX * 2];
    char cwd[PATH_MAX];
    if (!path){
        if (getcwd(cwd, sizeof(cwd)) == NULL) {
            print_red("getcwd error");
            return NULL;
        }
    }
    else{
        strcpy(cwd, path);
    }
    snprintf(full_path, sizeof(full_path), "%s/%s", cwd, relative_path);
    // printf("%s\n", full_path);
    char resolved_path[PATH_MAX];
    if (realpath(full_path, resolved_path) == NULL) {
        print_red("realpath error");
        return NULL;
    }
    char* actual_path = malloc(PATH_MAX);
    strcpy(actual_path, resolved_path);
    return actual_path;

}
void print_color_coded_name(char* filepath, char* filename, int flag) {
    // printf("%s\n", filepath);
    if(!flag){
        if (is_dir(filepath)) {
            printf("\033[1;34m%s\033[0m\n", filename); // Blue for directories
        } 
        else {
            struct stat file_stat;
            if (stat(filepath, &file_stat) == 0 && file_stat.st_mode & S_IXUSR) {
                printf("\033[1;32m%s\033[0m\n", filename); // Green for executables
            } 
            else {
                printf("\033[0;37m%s\033[0m\n", filename); // White for regular files
            }
        }
    }
    else{
        if (is_dir(filepath)) {
            printf("\033[1;34m%s\033[0m\n", filepath); // Blue for directories
        } 
        else {
            struct stat file_stat;
            if (stat(filepath, &file_stat) == 0 && file_stat.st_mode & S_IXUSR) {
                printf("\033[1;32m%s\033[0m\n", filepath); // Green for executables
            } 
            else {
                printf("\033[0;37m%s\033[0m\n", filepath); // White for regular files
            }
        }

    }
}
void print_file_info(char* filepath, char *filename) {
    // printf("%s\n", filename);
    struct stat file_stat;
    
    // Get file stats
    if (stat(filepath, &file_stat) == -1) {
        print_red("stat error\n"); 
        return;
    }

    // File type and permissions
    printf((S_ISDIR(file_stat.st_mode)) ? "d" : "-");
    printf((file_stat.st_mode & S_IRUSR) ? "r" : "-");
    printf((file_stat.st_mode & S_IWUSR) ? "w" : "-");
    printf((file_stat.st_mode & S_IXUSR) ? "x" : "-");
    printf((file_stat.st_mode & S_IRGRP) ? "r" : "-");
    printf((file_stat.st_mode & S_IWGRP) ? "w" : "-");
    printf((file_stat.st_mode & S_IXGRP) ? "x" : "-");
    printf((file_stat.st_mode & S_IROTH) ? "r" : "-");
    printf((file_stat.st_mode & S_IWOTH) ? "w" : "-");
    printf((file_stat.st_mode & S_IXOTH) ? "x" : "-");

    // Number of links
    printf(" %lu", file_stat.st_nlink);

    // Owner and group
    struct passwd *pw = getpwuid(file_stat.st_uid);
    struct group  *gr = getgrgid(file_stat.st_gid);
    printf(" %s %s", pw->pw_name, gr->gr_name);

    // File size
    printf(" %5ld", file_stat.st_size);

    // Last modification time
    char timebuf[80];
    struct tm *timeinfo = localtime(&file_stat.st_mtime);
    strftime(timebuf, sizeof(timebuf), "%b %d %H:%M", timeinfo);
    printf(" %s ", timebuf);

    // File name
    print_color_coded_name(filepath, filename, 0);
}



void reveal(char *input, char* prev_dir) {
    char* home_dir = "/mnt/c/Users/Nikhil/Desktop/OSN/MP1";
    int num_tokens = token_count(input);
    char** tokens = input_to_tokens(input, num_tokens);
    if (strcmp(tokens[0], "reveal")){
        print_red("invalid input");
        return;
    }
    int l_flag = 0;
    int a_flag = 0;
    int path_check = 0;
    if (num_tokens == 1){
        if (!strcmp(tokens[0], "reveal")){    // reveal
            char cwd[PATH_MAX];
            if (getcwd(cwd, sizeof(cwd)) == NULL) {
                print_red("getcwd error");
                return;
            }
            char* dir = malloc(PATH_MAX);
            strcpy(dir, cwd);
            int num_files = find_num_files(dir, 0);
            if (num_files == 0){
                print_red("empty directory");
                return;
            }
            char** filenames = get_filenames(dir, 0);
            merge_sort_filenames(filenames, num_files);
            printf("total %d\n", total_blocks);
            for (int i = 0; i < num_files; i++){
                char* actual_path = get_actual_path(filenames[i], dir);
                print_color_coded_name(actual_path, filenames[i], 0);
                free(actual_path);
            }
            free(dir);
        }
        
    }
    else{
        for (int i = 1; i < num_tokens; i++){
            if (tokens[i][0] == '-'){   // flag processing
                if (is_substring("l", tokens[i])){
                    l_flag = 1;
                }
                if (is_substring("a", tokens[i])){
                    a_flag = 1;
                }
                if (!l_flag && !a_flag){  // invalid flags
                    char *other_letters = "bcdefghikmnopqrstuvwxyz";
                    for (int j = 0; j < strlen(other_letters); j++){
                        char* s = malloc(2);
                        s[0] = other_letters[j];
                        s[1] = '\0';
                        if (is_substring(s, tokens[i])){
                            print_red("invalid flag");
                            free(s);
                            return;
                        }

                    }
                }
                if (!strcmp(tokens[i], "-")){   // reveal - or reveal -a - or reveal -l - or reveal -al -
                    if(strlen(prev_dir) == 0){
                        print_red("old pwd not set");   
                        return;
                    }
                    else{
                        int num_files = find_num_files(prev_dir, a_flag);
                        if (num_files == 0){
                            print_red("empty directory");
                            return;
                        }
                        char** filenames = get_filenames(prev_dir, a_flag);
                        merge_sort_filenames(filenames, num_files);
                        printf("total %d\n", total_blocks);
                        for (int j = 0; j < num_files; j++){
                            char* actual_path = get_actual_path(filenames[j], prev_dir);
                            if(l_flag){
                                print_file_info(actual_path, filenames[j]);
                            }
                            else{
                                print_color_coded_name(actual_path, filenames[j], 0);
                            }
                            free(actual_path);
                        }
                        for (int j = 0; j < num_files; j++){
                            free(filenames[j]);
                        }
                        free(filenames);
                        for (int j = 0; j < num_tokens; j++){
                            free(tokens[j]);
                        }
                        free(tokens);
                        return;
                    }
                }
                if (i == num_tokens - 1){   //reveal -a or reveal -l or reveal -la
                    char cwd[PATH_MAX];
                    if (getcwd(cwd, sizeof(cwd)) == NULL) {
                        print_red("getcwd error");
                        return;
                    }
                    char* dir = malloc(PATH_MAX);
                    strcpy(dir, cwd);
                    int num_files = find_num_files(dir, a_flag);
                    if (num_files == 0){
                        print_red("empty directory");
                        return;
                    }
                    char** filenames = get_filenames(dir, a_flag);
                    merge_sort_filenames(filenames, num_files);
                    printf("total %d\n", total_blocks);
                    for (int j = 0; j < num_files; j++){
                        char* actual_path = get_actual_path(filenames[j], dir);
                        if (l_flag){
                            print_file_info(actual_path, filenames[j]);
                        }
                        else{
                            print_color_coded_name(actual_path, filenames[j], 0);
                        }
                        free(actual_path);
                    }
                    for (int j = 0; j < num_files; j++){
                        free(filenames[j]);
                    }
                    free(filenames);
                    free(dir);
                    return;
                }
                
            }
        }
        for (int i = 1; i < num_tokens; i++){
            if (tokens[i][0] == '/' || tokens[i][0] == '.'){   // reveal <abs_path> or reveal <flags> <abs_path> or reveal <rel_path> or reveal <flags> <rel_path>  (here rel_path starts with '.')
                path_check = 1;
                if (!is_dir(tokens[i])){  // reveal <abs_path> where abs_path is of a file
                    if(l_flag){
                        print_file_info(tokens[i], tokens[i]);
                    }
                    else{
                        print_color_coded_name(tokens[i], tokens[i], 0);
                    }  
                    return;
                }
                int num_files = find_num_files(tokens[i], a_flag);
                if (num_files == 0){
                    print_red("empty directory");
                    return;
                }
                char** filenames = get_filenames(tokens[i], a_flag);
                printf("total %d\n", total_blocks);
                merge_sort_filenames(filenames, num_files);
                for (int j = 0; j < num_files; j++){
                    char* actual_path = get_actual_path(filenames[j], tokens[i]);
                    if(l_flag){
                        print_file_info(actual_path, filenames[j]);
                    }
                    else{
                        print_color_coded_name(actual_path, filenames[j], 0);
                    }
                    free(actual_path);
                }
                for (int j = 0; j < num_files; j++){
                    free(filenames[j]);
                }
                free(filenames);

            }
            else if(tokens[i][0] == '~'){   // reveal ~ or reveal <flags> ~ or reveal <rel_path> or reveal <flags> <rel_path> (here rel_path starts with '~')
                path_check = 1;
                if (!strcmp(tokens[i], "~")){
                    int num_files = find_num_files(home_dir, a_flag);
                    if (num_files == 0){
                        print_red("empty directory\n");
                        return;
                    }
                    char** filenames = get_filenames(home_dir, a_flag);
                    merge_sort_filenames(filenames, num_files);
                    printf("total %d\n", total_blocks);
                    for (int j = 0; j < num_files; j++){
                        char* actual_path = get_actual_path(filenames[j], home_dir);
                        if (l_flag){
                            print_file_info(actual_path, filenames[j]);
                        }
                        else{
                            print_color_coded_name(actual_path, filenames[j], 0);
                        }
                        free(actual_path);
                    }
                    for (int j = 0; j < num_files; j++){
                        free(filenames[j]);
                    }
                    free(filenames);
                }
                else{
                    char* dir = calloc(PATH_MAX, 1);
                    slice(tokens[i], dir, 2, strlen(tokens[i]));
                    if (!is_dir(dir)){
                        if(l_flag){
                            print_file_info(dir, tokens[i]);
                        }
                        else{
                            print_color_coded_name(dir, tokens[i], 0);
                        }  
                        free(dir);
                        return;
                    }
                    int num_files = find_num_files(dir, a_flag);
                    if (num_files == 0){
                        print_red("empty directory\n");
                        return;
                    }
                    char** filenames = get_filenames(dir, a_flag);
                    merge_sort_filenames(filenames, num_files);
                    printf("total %d\n", total_blocks);
                    for (int j = 0; j < num_files; j++){
                        char* actual_path = get_actual_path(filenames[j], dir);
                        if(l_flag){
                            print_file_info(actual_path, filenames[j]);
                        }
                        else{
                            print_color_coded_name(actual_path, filenames[j], 0);
                        }
                        free(actual_path);       
                    }
                    for (int j = 0; j < num_files; j++){
                        free(filenames[j]);
                    }
                    free(filenames);
                    free(dir);
                }
            }
            else if (tokens[i][0] == '-'){  // ignore '-' since we already dealt with them (except if - is at the end)
                if (i != num_tokens - 1){
                    continue;
                }
            }
            else{   // reveal <dirname> or reveal <flags> <dirname> where dirname is in current directory
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) == NULL) {
                    print_red("getcwd error\n");
                    return;
                }
                char* dir = malloc(PATH_MAX);
                strcpy(dir, cwd);
                char* actual_path = get_actual_path(tokens[i], dir);
                if (is_dir(actual_path)){
                    int num_files = find_num_files(actual_path, a_flag);
                    if (num_files == 0){
                        print_red("empty directory\n");
                        return;
                    }
                    char** filenames = get_filenames(actual_path, a_flag);
                    merge_sort_filenames(filenames, num_files);
                    for (int j = 0; j < num_files; j++){
                        char* Actual_path = get_actual_path(filenames[j],actual_path);
                        if (l_flag){
                            print_file_info(Actual_path, filenames[j]);
                        }
                        else{
                            print_color_coded_name(Actual_path, filenames[j], 0);
                        }
                        free(Actual_path);
                    }
                    for (int j = 0; j < num_files; j++){
                        free(filenames[j]);
                    }
                    free(filenames);

                }
                else{
                    if(l_flag){
                        print_file_info(tokens[i], tokens[i]);
                    }
                    else{
                        print_color_coded_name(tokens[i], tokens[i], 0);
                    }  
                }
                free(dir);
                free(actual_path);
                return;
            }
            if (i == num_tokens - 1 && !path_check){ //reveal <filename> or reveal <flags> <filename> where filename is in current directory
                char cwd[PATH_MAX];
                if (getcwd(cwd, sizeof(cwd)) == NULL) {
                    print_red("getcwd error");
                    return;
                }
                char* dir = malloc(PATH_MAX);
                strcpy(dir, cwd);
                int num_files = find_num_files(dir, a_flag);
                if (num_files == 0){
                    print_red("empty directory\n");
                    return;
                }
                char** filenames = get_filenames(dir, a_flag);
                merge_sort_filenames(filenames, num_files);
                printf("total %d\n", total_blocks);
                for (int j = 0; j < num_files; j++){
                    char* actual_path = get_actual_path(filenames[j], dir);
                    if (l_flag){
                        print_file_info(actual_path, filenames[j]);
                    }
                    else{
                        print_color_coded_name(actual_path, filenames[j], 0);
                    }
                    free(actual_path);
                }
                for (int j = 0; j < num_files; j++){
                    free(filenames[j]);
                }
                free(filenames);
                free(dir);
            }
        }
    }
    for (int i = 0; i < num_tokens; i++){
        free(tokens[i]);
    }
    free(tokens);
}


// int main(void){
//     char input[4096];
//     if (!fgets(input, sizeof(input), stdin)) {
//         printf("Error reading input.\n");
//         exit(1);
//     }
//     size_t len = strlen(input);
//     if (len > 0 && input[len - 1] == '\n') {
//         input[len - 1] = '\0';
//     }
//     char *cleaned_input = remove_extra_spaces_and_tabs(input);
//     reveal(cleaned_input, NULL);
//     free(cleaned_input);
// }