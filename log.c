#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include "all_functions.h"
#define MAX_LEN 15
Node* PushFront(char* c,Node*L){
    Node*newnode=malloc(sizeof(Node));
    newnode->key = malloc(4096);
    strcpy(newnode->key,c);
    if(!L){
        newnode->next=NULL;
        newnode->prev=NULL;
        L=newnode;
    }
    else{
        newnode->next=L;
        newnode->prev=NULL;
        L->prev=newnode;
        L=newnode;
    }
    return L;
}

Node* PopBack(Node*L){
    if(!L){
        print_red("Linked list is empty");
    }
    else{
        Node*temp = L;
        while(temp->next!=NULL){
            temp = temp->next;
        }
        temp->prev->next = NULL;
        temp->prev = NULL;
        free(temp->key);
        free(temp);
    }
    return L;
}
void print(Node* L){
    if (!L){
        return;
    }
    Node* temp=L;
    while(temp->next!=NULL){
        temp = temp->next;
    }
    while(temp!=NULL){
        printf("%s\n",temp->key);
        temp = temp->prev;
    }
}
void free_linked_list(Node* L) {
    Node* temp;
    while (L) {
        temp = L;
        L = L->next;
        free(temp->key);   
        free(temp);        
    }
}
Log* init_log() {
    Log *log = malloc(sizeof(Log));
    log->node = NULL;
    log->size = 0;
    log->capacity = MAX_LEN;
    return log;
}
void load_log_from_file(Log* log){
    FILE* f = fopen("/mnt/c/Users/Nikhil/Desktop/OSN/MP1/log.txt", "a");
    fclose(f);
    f = fopen("/mnt/c/Users/Nikhil/Desktop/OSN/MP1/log.txt", "r");
    int ind = 0;
    char str[4096];
    while (fgets(str, sizeof(str), f)) {
        size_t len = strlen(str);
        if (len > 0 && str[len - 1] == '\n') {
            str[len - 1] = '\0';
        } 
        log->node = PushFront(str, log->node);
        log->size++;
    }
    fclose(f);
}
void add_to_log(Log *log, char *command) {
    if (is_substring("log", command)){
        return;
    }
    if (log->size > 0) {
        if (!strcmp(command, log->node->key)){
            return;
        }
    }
    if (log->size == log->capacity) {
        PopBack(log->node);
        log->size--;
    }
    log->node = PushFront(command, log->node); 
    log->size++;
}
int log_execute(Log* log, int index, char* prev_dir){
    if (index > log->size){
        print_red("invalid index");
        return 1;
    }
    index--;
    char* input = malloc(4096);
    Node* temp = log->node;
    while (index){
        temp = temp->next;
        index--;
    }
    strcpy(input, temp->key);
    // printf("%s\n", input);
    char *cleaned_input = remove_extra_spaces_and_tabs(input);
    int num_tokens = token_count(cleaned_input);
    char **commands = parse_input(cleaned_input, num_tokens);
    free(cleaned_input);
    int num_non_empty_strings = find_num_non_empty_strings(commands, num_tokens);
    char **final_commands = remove_empty_strings(commands, num_tokens, num_non_empty_strings);
    for (int i = 0; i < num_non_empty_strings; i++) {
        char*comm = replace_alias(final_commands[i]);
        cont = execute_command(comm, prev_dir, log);
        if (!cont){
            break;
        }
    }
    for (int i = 0; i < num_non_empty_strings; i++) {
        free(final_commands[i]);
    }
    free(final_commands);
    free(input);
    return cont;
}
void save_log_to_file(Log* log){
    FILE* f = fopen("/mnt/c/Users/Nikhil/Desktop/OSN/MP1/log.txt", "w");
    Node*temp = log->node;
    while(temp->next!=NULL){
        temp = temp->next;
    }
    while(temp){
        fprintf(f, "%s\n", temp->key);
        temp = temp->prev;
    }
    fclose(f);
}
int log_controller(char* command, Log* log, char* prev_dir){
    int cont = 1;
    char* cleaned = remove_extra_spaces_and_tabs(command);
    int num_tokens = token_count(cleaned);
    char** tokens = input_to_tokens(cleaned, num_tokens);
    if (num_tokens == 1){
        // printf("%d\n", log->size);
        print(log->node);
    }
    else if(num_tokens == 2){
        if (!strcmp(tokens[1], "purge")){
            free_linked_list(log->node);
            log->node = NULL;
            log->size = 0;
        }
        else{
            print_red("invalid command");
            return 1;
        }
    }
    else if (num_tokens == 3){
        if (!strcmp(tokens[1], "execute")){
            int index = atoi(tokens[2]);
            cont = log_execute(log, index, prev_dir);
        }
        else{
            print_red("invalid command");
            return 1;
        }
    }
    else{
        print_red("invalid command");
        return 1;
    }
    
    for(int i = 0; i < num_tokens; i++){
        free(tokens[i]);
    }
    free(tokens);
    free(cleaned);
    return cont;
 }
