#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666

int main(void) {
    int                lfd, cfd, res;
    char               buf[BUFSIZ], clie_IP[BUFSIZ]; // 4k或者8k
    struct sockaddr_in serv_addr, clie_addr;
    lfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
    if (lfd == -1) {
        perror("socket error!");
        exit(1);
    }
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(SERV_PORT); // 主机转网络字节序
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr.s_addr); // 点分十进制转网络字节序

    res = bind(lfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (res == -1) {
        perror("bind error!");
        exit(1);
    }
    listen(lfd, 128); // 最大同时连接数
    if (res == -1) {
        perror("listen error!");
        exit(1);
    }
    int       i, n;
    pid_t     pid;
    socklen_t clie_addr_len; // 套接字长度
    while (1) {
        clie_addr_len = sizeof(clie_addr);
        cfd = accept(lfd, (struct sockaddr*) &clie_addr, &clie_addr_len);
        if (cfd == -1) {
            perror("accept error!");
            exit(1);
        }
        printf("client IP: %s, client port: %d\n",
            inet_ntop(
                AF_INET, &clie_addr.sin_addr.s_addr, clie_IP, sizeof(clie_IP)),
            ntohs(clie_addr.sin_port));
        pid = fork();
        if (pid == 0) {
            close(lfd);
            break;
        } else if (pid == -1) {
            perror("fork error");
            exit(1);
        } else {
            close(cfd);
        }
    }
    // 跳出循环的子进程
    if (pid == 0) {
        while (1) {
            n = read(cfd, buf, sizeof(buf));
            if (n == 0) { // client close
                printf("close port = %d\n", ntohs(clie_addr.sin_port));
                close(cfd);
                exit(1);
            } else if (n == -1) {
                perror("read error");
                exit(1);
            } else {
                for (i = 0; i < n; i++) {
                    buf[i] = toupper(buf[i]);
                }
                write(1, buf, n);
                write(cfd, buf, n);
            }
        }
    }

    return 0;
}
