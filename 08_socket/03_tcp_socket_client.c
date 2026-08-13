#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 8080
/*
1.创建套接字socket()
2.调用connect函数与服务器连接
3.从键盘输入数据
4.从服务器读取数据
5.close
*/
int main(void) {
    // 1.
    int                cfd;
    struct sockaddr_in serv_addr;
    socklen_t          serv_addr_len;
    // socket: input buffer / output buffer
    // 文件描述符：是一个整数，也是访问一个缓冲区的标识
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    // 初始化socket参数
    memset(&serv_addr, 0, sizeof(serv_addr)); // 初始化
    serv_addr.sin_family = AF_INET;           // IPV4协议
    serv_addr.sin_port   = htons(SERV_PORT);  // 服务器端口
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr.s_addr); // 点分十进制转网络字节序
    // 2.
    //  服务端的IP+PORT需要指定，客户端的IP，端口可以随机选择一个。
    int ret = connect(cfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

    char buf[BUFSIZ];
    int  n;
    // while (1) {
    // 3/4.
    printf("please input a string: ");
    fgets(buf, sizeof(buf), stdin); // 读一行 但是会自动加一个换行符
    // 去掉换行符
    int len = strlen(buf);
    if (len >= 1 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
    write(cfd, buf, strlen(buf)); // 写入到文件描述符 写缓冲区
    // n = read(cfd, buf, sizeof(buf));  //返回从服务器读出的字节数
    // write(STDOUT_FILENO, buf, n);     //把读入的数据写到屏幕
    // }
    // 5.
    shutdown(cfd, SHUT_WR);
    sleep(100);
    return 0;
}
