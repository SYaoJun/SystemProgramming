#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define PORT 8080

void handle_client_disconnection(int client_fd) {
    printf("Client (fd: %d) disconnected\n", client_fd);
    close(client_fd); // Close the client file descriptor
}

void set_non_blocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    fcntl(sockfd, F_SETFL, flags | O_NONBLOCK);
}

int main() {
    int                server_fd, epoll_fd;
    struct sockaddr_in server_addr;

    // Create server socket
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) < 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }
    set_non_blocking(server_fd);

    // Bind address and port
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr))
        < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Start listening
    if (listen(server_fd, 5) < 0) {
        perror("listen failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Create epoll instance
    if ((epoll_fd = epoll_create1(0)) < 0) {
        perror("epoll_create1 failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // Add server_fd to epoll
    struct epoll_event ev, events[MAX_EVENTS];
    ev.events  = EPOLLIN; // Listen for new connections
    ev.data.fd = server_fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, server_fd, &ev) < 0) {
        perror("epoll_ctl failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Server listening on port %d\n", PORT);

    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds < 0) {
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < nfds; i++) {
            if (events[i].data.fd == server_fd) {
                // Handle new connection
                int client_fd = accept(server_fd, NULL, NULL);
                if (client_fd < 0) {
                    perror("accept failed");
                    continue;
                }
                set_non_blocking(client_fd);

                ev.events = EPOLLIN
                    | EPOLLRDHUP; // Monitor for readability and peer close
                ev.data.fd = client_fd;
                if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) < 0) {
                    perror("epoll_ctl add client failed");
                    close(client_fd);
                    continue;
                }

                printf("New client connected (fd: %d)\n", client_fd);
            } else {
                // Handle client event
                int client_fd = events[i].data.fd;

                if (events[i].events & (EPOLLHUP | EPOLLRDHUP)) {
                    // Client disconnected
                    puts("from epollhup");
                    handle_client_disconnection(client_fd);
                    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                } else if (events[i].events & EPOLLIN) {
                    // Handle data sent by client
                    char    buffer[1024] = { 0 };
                    ssize_t bytes_read
                        = read(client_fd, buffer, sizeof(buffer));
                    if (bytes_read > 0) {
                        printf("Received from client (fd: %d): %s\n", client_fd,
                            buffer);
                    } else if (bytes_read == 0) {
                        // Client closed the connection
                        puts("from read 0");
                        handle_client_disconnection(client_fd);
                        epoll_ctl(epoll_fd, EPOLL_CTL_DEL, client_fd, NULL);
                    } else {
                        perror("read failed");
                    }
                }
            }
        }
    }

    close(server_fd);
    close(epoll_fd);
    return 0;
}
