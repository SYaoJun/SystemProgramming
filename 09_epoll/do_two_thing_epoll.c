/*
Usage:
./do_two_thing_epoll
nc localhost 8080
# 查看服务器监听的端口
netstat -apn | grep do_two_thing_epoll
*/
#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define PORT 8080

void set_nonblocking(int sockfd) {
    int flags = fcntl(sockfd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
    if (fcntl(sockfd, F_SETFL, flags | O_NONBLOCK) == -1) {
        perror("fcntl");
        exit(EXIT_FAILURE);
    }
}

int main() {
    int                listen_sock, conn_sock, nfds, epollfd, n;
    struct epoll_event ev, events[MAX_EVENTS];
    struct sockaddr_in addr;
    char               buffer[1024];

    // 创建监听套接字
    listen_sock = socket(AF_INET, SOCK_STREAM, 0);
    if (listen_sock == -1) {
        perror("socket");
        exit(EXIT_FAILURE);
    }

    set_nonblocking(listen_sock);

    memset(&addr, 0, sizeof(addr));
    addr.sin_family      = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port        = htons(PORT);

    if (bind(listen_sock, (struct sockaddr*) &addr, sizeof(addr)) == -1) {
        perror("bind");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    if (listen(listen_sock, SOMAXCONN) == -1) {
        perror("listen");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    // 创建 epoll 实例
    epollfd = epoll_create1(0);
    if (epollfd == -1) {
        perror("epoll_create1");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    // 将监听套接字添加到 epoll 实例中
    ev.events  = EPOLLIN;
    ev.data.fd = listen_sock;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, listen_sock, &ev) == -1) {
        perror("epoll_ctl: listen_sock");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    // 将标准输入添加到 epoll 实例中
    ev.events  = EPOLLIN;
    ev.data.fd = STDIN_FILENO;
    if (epoll_ctl(epollfd, EPOLL_CTL_ADD, STDIN_FILENO, &ev) == -1) {
        perror("epoll_ctl: stdin");
        close(listen_sock);
        exit(EXIT_FAILURE);
    }

    while (1) {
        nfds = epoll_wait(epollfd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait");
            close(listen_sock);
            exit(EXIT_FAILURE);
        }

        for (n = 0; n < nfds; ++n) {
            if (events[n].data.fd == listen_sock) {
                // 处理新的客户端连接
                conn_sock = accept(listen_sock, NULL, NULL);
                if (conn_sock == -1) {
                    perror("accept");
                    continue;
                }
                set_nonblocking(conn_sock);
                ev.events  = EPOLLIN | EPOLLET;
                ev.data.fd = conn_sock;
                if (epoll_ctl(epollfd, EPOLL_CTL_ADD, conn_sock, &ev) == -1) {
                    perror("epoll_ctl: conn_sock");
                    close(conn_sock);
                    continue;
                }
            } else if (events[n].data.fd == STDIN_FILENO) {
                // 处理标准输入
                ssize_t count = read(STDIN_FILENO, buffer, sizeof(buffer));
                if (count == -1) {
                    perror("read");
                    exit(EXIT_FAILURE);
                } else if (count == 0) { // zero indicates end of file
                    // 如果是在Linux标准输入上键入Ctrl+D，则表示EOF
                    printf("stdin closed\n");
                    exit(EXIT_SUCCESS);
                } else {
                    buffer[count] = '\0';
                    printf("Read from stdin: %s", buffer);
                }
            } else {
                // 处理客户端请求
                ssize_t count = read(events[n].data.fd, buffer, sizeof(buffer));
                if (count == -1) {
                    if (errno != EAGAIN) {
                        perror("read");
                        close(events[n].data.fd);
                    }
                } else if (count == 0) {
                    // 客户端关闭连接
                    close(events[n].data.fd);
                } else {
                    // 处理客户端数据
                    buffer[count] = '\0';
                    printf("Read from client: %s", buffer);
                    // 回送数据给客户端
                    if (write(events[n].data.fd, buffer, count) == -1) {
                        perror("write");
                        close(events[n].data.fd);
                    }
                }
            }
        }
    }

    close(listen_sock);
    return 0;
}
