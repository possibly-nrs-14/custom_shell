#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/utsname.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <string.h> 
#include <limits.h>
#include <pwd.h>
#include "all_functions.h"
char* get_username(uid_t uid){
    struct passwd *pwd = getpwuid(uid);
    if (pwd) {
        return pwd->pw_name;
    } 
    else {
        printf("error in getting username\n");
        exit(0);
    }
}
char* get_sysname(){
    char*hostname = calloc(HOST_NAME_MAX + 1, 1);
    if( gethostname(hostname, sizeof(hostname)) == -1){
        printf("error in getting hostname\n");
    }
    return hostname;
}
void slice(char* str, char* result, size_t start, size_t end) {
    strncpy(result, str + start, end - start);
}
int is_substring(char* str1, char* str2){  // checks if str1 is a substr of str2
    if (strlen(str1) > strlen(str2)){
        return 0;
    }
    int p1 = 0;
    int p2 = 0;
    int check = 0;
    while(p1 < strlen(str1) && p2 < strlen(str2)){
        if (str1[p1] == str2[p2]){
            if (!check){
                check = 1;
            }
            p1++;
            p2++;
        }
        else{
            if (check){
                check = 0;  
                p1 = 0;
                if (str1[p1] == str2[p2]){
                    check = 1;
                    p1++;
                }
            }
        p2++;
        }
    }
    if (p1 < strlen(str1)){
        check = 0;
    }
    return check;
}
char* get_curr_directory(){
    char* home_dir = "/mnt/c/Users/Nikhil/Desktop/OSN/MP1";
    size_t home_dir_len = strlen(home_dir);
    char buff[PATH_MAX];
    if (!getcwd(buff, sizeof(buff))){
        printf("error in getting directory\n");
        exit(0);
    }
    char* dir = calloc(PATH_MAX, 1);
    if (is_substring(home_dir, buff)){
        strcat(dir, "~");
        char* rel_dir = calloc(50, 1);
        slice(buff, rel_dir, home_dir_len , strlen(buff));
        strcat(dir, rel_dir);
        free(rel_dir);
    }
    else{
        strcat(dir, buff);
    }
    return dir;

}
char* create_prompt(char* username, char* hostname, int flag, char* time_info){
    char* prompt = calloc(100, 1);
    strcat(prompt, "<");
    strcat(prompt, username);
    strcat(prompt, "@");
    strcat(prompt, hostname);
    strcat(prompt, ":");
    char* dir = get_curr_directory();
    strcat(prompt, dir);
    free(dir);
    if (flag && time_info){
        strcat(prompt, time_info);
    }
    strcat(prompt, ">");
    return prompt;
}
void print_red(char* str){
    printf("\033[1;31m%s\033[0m\n", str);
}


// int main(void){
//     char*hostname = get_sysname();
//     uid_t uid = getuid();
//     char* username = get_username(uid);
//     char* prompt = create_prompt(username, hostname);
//     printf("%s\n", prompt);
//     free(hostname);
//     free(prompt);
// }