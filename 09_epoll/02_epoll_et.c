#include <arpa/inet.h>
#include <assert.h>
#include <errno.h>
#include <fcntl.h>
#include <libgen.h>
#include <netinet/in.h>
#include <pthread.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/socket.h>
#include <unistd.h>
#define MAX_EVENT_NUMBER 1024
#define BUFFER_SIZE 10
/* Set the file descriptor to non-blocking */
int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
/* Register EPOLLIN on file descriptor fd into the epoll kernel event table
indicated by epollfd; the parameter enable_et specifies whether to enable ET
mode for fd */
void addfd(int epollfd, int fd, bool enable_et) {
    struct epoll_event event;
    event.data.fd = fd;
    event.events  = EPOLLIN;
    if (enable_et) {
        event.events |= EPOLLET;
    }
    epoll_ctl(epollfd, EPOLL_CTL_ADD, fd, &event);
    setnonblocking(fd);
}
/* Workflow of LT mode */
void lt(struct epoll_event* events, int number, int epollfd, int listenfd) {
    char buf[BUFFER_SIZE];
    for (int i = 0; i < number; i++) {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd) {
            puts("listen fd");
            struct sockaddr_in client_address;
            socklen_t          client_addrlength = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*) &client_address,
                &client_addrlength);
            addfd(epollfd, connfd, false); /* Disable ET mode for connfd */
        } else if (events[i].events & EPOLLIN) {
            /* This code is triggered as long as there is unread data in the
            socket read buffer */
            printf("event trigger once\n");
            memset(buf, '\0', BUFFER_SIZE);
            int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);
            if (ret <= 0) {
                close(sockfd);
                continue;
            }
            printf("get %d bytes of content:%s\n", ret, buf);
        } else {
            printf("something else happened\n");
        }
    }
}
/* Workflow of ET mode */
void et(struct epoll_event* events, int number, int epollfd, int listenfd) {
    char buf[BUFFER_SIZE];
    for (int i = 0; i < number; i++) {
        int sockfd = events[i].data.fd;
        if (sockfd == listenfd) {
            puts("listen fd");
            struct sockaddr_in client_address;
            socklen_t          client_addrlength = sizeof(client_address);
            int connfd = accept(listenfd, (struct sockaddr*) &client_address,
                &client_addrlength);
            addfd(epollfd, connfd, true); /* Enable ET mode for connfd */
        } else if (events[i].events & EPOLLIN) {
            /* This code will not be triggered repeatedly, so we read data in a
            loop to ensure all data in the socket read buffer is read out */
            printf("event trigger once\n");
            while (1) {
                memset(buf, '\0', BUFFER_SIZE);
                int ret = recv(sockfd, buf, BUFFER_SIZE - 1, 0);
                if (ret < 0) {
                    /* For non-blocking IO, the following condition being true
                    means all data has been read. After that, epoll can trigger
                    EPOLLIN on sockfd again to drive the next read operation */
                    if ((errno == EAGAIN) || (errno == EWOULDBLOCK)) {
                        printf("read later\n");
                        break;
                    }
                    close(sockfd);
                    break;
                } else if (ret == 0) {
                    close(sockfd);
                } else {
                    printf("get %d bytes of content:%s\n", ret, buf);
                }
            }
        } else {
            printf("something else happened\n");
        }
    }
}
int main(int argc, char* argv[]) {
    if (argc <= 2) {
        printf("usage:%s ip_address port_number\n", basename(argv[0]));
        return 1;
    }
    const char*        ip   = argv[1];
    int                port = atoi(argv[2]);
    int                ret  = 0;
    struct sockaddr_in address;
    bzero(&address, sizeof(address));
    address.sin_family = AF_INET;
    inet_pton(AF_INET, ip, &address.sin_addr);
    address.sin_port = htons(port);
    int listenfd     = socket(PF_INET, SOCK_STREAM, 0);

    assert(listenfd >= 0);
    ret = bind(listenfd, (struct sockaddr*) &address, sizeof(address));
    assert(ret != -1);
    ret = listen(listenfd, 5);
    assert(ret != -1);
    struct epoll_event events[MAX_EVENT_NUMBER];
    int                epollfd = epoll_create(5);
    assert(epollfd != -1);
    addfd(epollfd, listenfd, true);
    while (1) {
        int ret = epoll_wait(epollfd, events, MAX_EVENT_NUMBER, -1);
        puts("trigger epoll wait...");
        if (ret < 0) {
            printf("epoll failure\n");
            break;
        }
        // lt(events, ret, epollfd, listenfd); /* Use LT mode */

        et(events, ret, epollfd, listenfd); /* Use ET mode */
    }
    close(listenfd);
    return 0;
}
