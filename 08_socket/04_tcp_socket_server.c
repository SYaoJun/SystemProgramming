#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"

int main(int argc, char** argv) {
    int server_port = 0;
    if (argc < 2 || argc > 2) {
        puts("usage: ./a.out port");
        return 0;
    }
    server_port = atoi(argv[1]);

    int                lfd, cfd;
    char               buf[BUFSIZ], client_IP[BUFSIZ]; // 4k或者8k
    struct sockaddr_in serv_addr, clie_addr;
    // socket是一个input和output缓冲区
    // 1.
    // lfd, 专门用来监听，不用来读写真实的数据 socket 文件描述符：37
    lfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(server_port);
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr.s_addr); // 点分十进制转网络字节序

    // 绑定端口 类似于将上课的A5教室定下来，而客户端就是学生，学生需要知道教室A5
    // 绑定提供服务的接口
    // 2.
    bind(lfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    // 监听端口
    // 3.
    listen(lfd, 128);        // 最大同时连接数
    socklen_t clie_addr_len; // 套接字长度
    clie_addr_len = sizeof(clie_addr);
    // 接收请求 阻塞调用 等待
    // 4.
    cfd = accept(lfd, (struct sockaddr*) &clie_addr, &clie_addr_len);
    // 返回的cfd才是真正用来读写数据的
    printf("client IP: %s, client port: %d\n",
        inet_ntop(
            AF_INET, &clie_addr.sin_addr.s_addr, client_IP, sizeof(client_IP)),
        ntohs(clie_addr.sin_port));
    int i, n;
    // 终端输入nc 127.0.0.1 6670 去发送数据
    // 服务端如何支持rest ful风格的请求get post put delete
    while (1) { // 一直读 一直写
        // 5/6
        n = read(cfd, buf, sizeof(buf));
        for (i = 0; i < n; i++) {
            buf[i] = toupper(buf[i]);
        }
        buf[i++] = '!';
        printf("recv a string: \n%s\n", buf);
        write(cfd, buf, n + 1);
        printf("\n");
    }
    // 7.
    // 文件描述符都需要关闭
    close(lfd);
    close(cfd);
    return 0;
}
