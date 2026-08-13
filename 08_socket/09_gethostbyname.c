#include <netdb.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>

int main() {
    int                sockfd, ret;
    struct sockaddr_in serv_addr;
    struct hostent*    server;

    // 创建socket
    sockfd = socket(AF_INET, SOCK_STREAM, 0);
    if (sockfd < 0) {
        perror("ERROR opening socket");
        exit(1);
    }

    // 解析服务器地址
    server = gethostbyname("localhost"); // 使用localhost作为示例
    if (server == NULL) {
        fprintf(stderr, "ERROR, no such host\n");
        exit(0);
    }

    // 构建服务器地址结构
    memset(&serv_addr, 0, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(3306); // mysql默认端口
    memcpy(&serv_addr.sin_addr.s_addr, server->h_addr, server->h_length);

    // 连接到服务器
    ret = connect(sockfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (ret < 0) {
        perror("ERROR connecting");
        exit(1);
    }

    printf("Connected to MySQL server\n");

    // ... 进行通信 ...

    // 关闭socket
    close(sockfd);
    return 0;
}
