#include <arpa/inet.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

int main() {
    int                server_fd;
    struct sockaddr_in address;
    int                opt     = 1;
    int                addrlen = sizeof(address);

    // 创建 socket 文件描述符
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0) {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // 绑定 socket 到特定的 IP 地址和端口
    address.sin_family = AF_INET;
    // address.sin_addr.s_addr = INADDR_ANY;  // 监听主机上的所有IP
    address.sin_addr.s_addr = inet_addr("127.0.0.1"); // 只监听 127.0.0.1
    address.sin_port        = htons(8080);            // 端口号

    if (bind(server_fd, (struct sockaddr*) &address, sizeof(address)) < 0) {
        perror("bind failed");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    // 开始监听
    if (listen(server_fd, 3) < 0) {
        perror("listen");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Listening on 127.0.0.1:8080\n");

    // 接受客户端连接
    int new_socket;
    if ((new_socket = accept(
             server_fd, (struct sockaddr*) &address, (socklen_t*) &addrlen))
        < 0) {
        perror("accept");
        close(server_fd);
        exit(EXIT_FAILURE);
    }

    printf("Connection accepted\n");

    // 处理连接...

    close(new_socket);
    close(server_fd);
    return 0;
}
