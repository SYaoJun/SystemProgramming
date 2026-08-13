#include <arpa/inet.h>
#include <errno.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>

#define MAX_EVENTS 10
#define BACKLOG 10

int create_and_bind(const char* ip, int port) {
    int listen_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_fd == -1) {
        perror("socket failed");
        return -1;
    }

    int yes = 1;
    if (setsockopt(listen_fd, SOL_SOCKET, SO_REUSEADDR, &yes, sizeof(yes))
        == -1) {
        perror("setsockopt failed");
        close(listen_fd);
        return -1;
    }

    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_port        = htons(port);
    addr.sin_addr.s_addr = inet_addr(ip);

    if (bind(listen_fd, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        perror("bind failed");
        close(listen_fd);
        return -1;
    }

    if (listen(listen_fd, BACKLOG) == -1) {
        perror("listen failed");
        close(listen_fd);
        return -1;
    }

    return listen_fd;
}

void handle_new_connection(int epoll_fd, int listen_fd) {
    struct sockaddr_in client_addr;
    socklen_t          client_addr_len = sizeof(client_addr);
    int                client_fd
        = accept(listen_fd, (struct sockaddr*) &client_addr, &client_addr_len);
    if (client_fd == -1) {
        perror("accept failed");
        return;
    }

    printf("Accepted new connection from %s:%d\n",
        inet_ntoa(client_addr.sin_addr), ntohs(client_addr.sin_port));

    // Set client_fd to non-blocking
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl failed");
        close(client_fd);
        return;
    }

    // Add the new client socket to epoll for monitoring
    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        perror("epoll_ctl failed");
        close(client_fd);
    }
}

int main() {
    // Create epoll instance
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        return -1;
    }

    // Listen on multiple IP addresses
    const char* ips[]   = { "192.168.1.100",
        "192.168.1.101" };                // List of IP addresses to listen on
    int         ports[] = { 8080, 8081 }; // Corresponding port list

    for (int i = 0; i < sizeof(ips) / sizeof(ips[0]); ++i) {
        int listen_fd = create_and_bind(ips[i], ports[i]);
        if (listen_fd == -1) {
            close(epoll_fd);
            return -1;
        }

        // Add the listening socket to the epoll instance
        struct epoll_event ev;
        ev.events  = EPOLLIN;
        ev.data.fd = listen_fd;

        if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, listen_fd, &ev) == -1) {
            perror("epoll_ctl failed");
            close(listen_fd);
            close(epoll_fd);
            return -1;
        }
    }

    // Loop waiting for events
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].events & EPOLLIN) {
                // Check if this is an event on the listening socket
                if (events[i].data.fd == listen_fd) {
                    handle_new_connection(epoll_fd, listen_fd);
                } else {
                    // Handle data from connected client
                    printf("Data from client: %d\n", events[i].data.fd);
                    // Logic for reading and processing data can be added here
                }
            }
        }
    }

    // Clean up resources
    close(epoll_fd);
    return 0;
}
