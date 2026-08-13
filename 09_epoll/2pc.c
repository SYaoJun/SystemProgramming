#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define PORT 8080
#define MAXLINE 1024

void send_message(int sockfd, const char* message) {
    send(sockfd, message, strlen(message), 0);
}

void receive_message(int sockfd, char* buffer) {
    int n     = read(sockfd, buffer, MAXLINE);
    buffer[n] = '\0';
}

int main() {
    int                sockfd;
    struct sockaddr_in servaddr;
    char               buffer[MAXLINE];

    // Create socket
    if ((sockfd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("Socket creation failed");
        exit(EXIT_FAILURE);
    }

    memset(&servaddr, 0, sizeof(servaddr));

    // Server information
    servaddr.sin_family      = AF_INET;
    servaddr.sin_port        = htons(PORT);
    servaddr.sin_addr.s_addr = INADDR_ANY;

    // Connect to server
    if (connect(sockfd, (struct sockaddr*) &servaddr, sizeof(servaddr)) < 0) {
        perror("Connection failed");
        exit(EXIT_FAILURE);
    }

    // Phase 1: Prepare
    send_message(sockfd, "PREPARE");
    receive_message(sockfd, buffer);
    printf("Server: %s\n", buffer);

    if (strcmp(buffer, "READY") == 0) {
        // Phase 2: Commit
        send_message(sockfd, "COMMIT");
        receive_message(sockfd, buffer);
        printf("Server: %s\n", buffer);
    } else {
        // Abort
        send_message(sockfd, "ABORT");
        receive_message(sockfd, buffer);
        printf("Server: %s\n", buffer);
    }

    close(sockfd);
    return 0;
}
