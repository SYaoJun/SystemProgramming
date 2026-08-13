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

    // 设置client_fd为非阻塞
    if (fcntl(client_fd, F_SETFL, O_NONBLOCK) == -1) {
        perror("fcntl failed");
        close(client_fd);
        return;
    }

    // 将新的客户端套接字添加到epoll监听
    struct epoll_event ev;
    ev.events  = EPOLLIN;
    ev.data.fd = client_fd;

    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, client_fd, &ev) == -1) {
        perror("epoll_ctl failed");
        close(client_fd);
    }
}

int main() {
    // 创建epoll实例
    int epoll_fd = epoll_create1(0);
    if (epoll_fd == -1) {
        perror("epoll_create1 failed");
        return -1;
    }

    // 监听多个IP地址
    const char* ips[]
        = { "192.168.1.100", "192.168.1.101" }; // 要监听的IP地址列表
    int ports[] = { 8080, 8081 };               // 对应的端口列表

    for (int i = 0; i < sizeof(ips) / sizeof(ips[0]); ++i) {
        int listen_fd = create_and_bind(ips[i], ports[i]);
        if (listen_fd == -1) {
            close(epoll_fd);
            return -1;
        }

        // 将监听套接字添加到epoll实例
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

    // 循环等待事件
    struct epoll_event events[MAX_EVENTS];
    while (1) {
        int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
        if (nfds == -1) {
            perror("epoll_wait failed");
            break;
        }

        for (int i = 0; i < nfds; ++i) {
            if (events[i].events & EPOLLIN) {
                // 检查是否是监听套接字上的事件
                if (events[i].data.fd == listen_fd) {
                    handle_new_connection(epoll_fd, listen_fd);
                } else {
                    // 处理已连接客户端的数据
                    printf("Data from client: %d\n", events[i].data.fd);
                    // 这里可以添加读取和处理数据的逻辑
                }
            }
        }
    }

    // 清理资源
    close(epoll_fd);
    return 0;
}
