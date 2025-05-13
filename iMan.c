#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <netdb.h>
#include <ctype.h>
#include "all_functions.h"


void iMan(char *command_name) {
    int sockfd;
    struct sockaddr_in server_addr;
    struct hostent *server;
    char buffer[10000];
    char request[4096];
    int bytes_received;
    char *host = "man.he.net";
    int port = 80; 
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        print_red("Error creating socket");
        return;
    }
    server = gethostbyname(host);
    if (!server) {
        print_red("Error: No such host");
        return;
    }
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family = AF_INET;
    server_addr.sin_port = htons(port);
    memcpy(&server_addr.sin_addr.s_addr, server->h_addr, server->h_length);
    if (connect(sockfd, (struct sockaddr *)&server_addr, sizeof(server_addr)) < 0) {
        print_red("Error connecting to the server");
        return;
    }
    sprintf(request, "GET /?topic=%s&section=all HTTP/1.1\r\n" "Host: man.he.net\r\n" "Connection: close\r\n\r\n", command_name);
    if (send(sockfd, request, strlen(request), 0) < 0) {
        print_red("Error sending request");
    }
    bytes_received = recv(sockfd, buffer, sizeof(buffer) - 1, 0);
    if (bytes_received < 0) {
        print_red("Error reading response");
        return;
    }
    buffer[bytes_received] = '\0';  
    char* decapitated = strstr(buffer, "</head>");
    printf("%s\n", decapitated + 8);
    close(sockfd);
}

void iMan_handler(char* command){
    int num_tokens = token_count(command);
    if (num_tokens < 2){
        print_red("invalid command");
        return;
    }
    char** tokens = input_to_tokens(command, num_tokens);
    iMan(tokens[1]);
    for(int i = 0; i < num_tokens; i++){
        free(tokens[i]);
    }
    free(tokens);
}

