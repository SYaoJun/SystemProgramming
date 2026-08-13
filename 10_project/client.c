#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 6666
/*
1.创建套接字
2.调用connect函数与服务器连接
3.从键盘write数据
4.从服务器read数据
5.close
*/
int main(void) {
    // 1.
    int                cfd;
    struct sockaddr_in serv_addr;
    socklen_t          serv_addr_len;
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    // 2.
    memset(&serv_addr, 0, sizeof(serv_addr)); // 初始化
    serv_addr.sin_family = AF_INET;           // IPV4协议
    serv_addr.sin_port   = htons(SERV_PORT);  // 服务器端口
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr.s_addr); // 点分十进制转网络字节序
    // 3.
    connect(cfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    // 4.
    char buf[BUFSIZ];
    int  n;
    while (1) {
        fgets(buf, sizeof(buf), stdin);  // 读一行 但是会自动加一个换行符
        write(cfd, buf, strlen(buf));    // 写入到文件描述符 写缓冲区
        n = read(cfd, buf, sizeof(buf)); // 返回从服务器读出的字节数
        write(STDOUT_FILENO, buf, n);    // 把读入的数据写到屏幕
    }
    // 5.
    close(cfd);
    return 0;
}
