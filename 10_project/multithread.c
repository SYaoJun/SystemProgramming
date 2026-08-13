#include <arpa/inet.h>
#include <ctype.h>
#include <pthread.h>
#include <stdio.h>
#include <stdlib.h>
#include <strings.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"
#define SERV_PORT 9999

struct s_info {
    struct sockaddr_in cliaddr;
    int                cfd;
};
void* do_work(void* arg) {                       // 子线程的工作内容
    char           buf[BUFSIZ], clie_IP[BUFSIZ]; // 4k或者8k
    int            i, n;
    struct s_info* ts = (struct s_info*) arg;
    while (1) {
        n = read(ts->cfd, buf, BUFSIZ);
        if (n == 0) {
            printf("the client %d closed ... \n", ts->cfd);
            break;
        }

        printf("client IP: %s, client port: %d\n",
            inet_ntop(AF_INET, &ts->cliaddr.sin_addr.s_addr, clie_IP,
                sizeof(clie_IP)),
            ntohs(ts->cliaddr.sin_port));
        // 小写转大写
        for (i = 0; i < n; i++) {
            buf[i] = toupper(buf[i]);
        }
        write(STDOUT_FILENO, buf, n);
        write(ts->cfd, buf, n);
    }
    close(ts->cfd);
    return (void*) 0;
}
int main(void) {
    int                lfd, cfd, res;
    pthread_t          tid;
    struct s_info      ts[256];
    int                i = 0;
    struct sockaddr_in serv_addr, clie_addr;
    lfd = socket(AF_INET, SOCK_STREAM, 0); // 创建套接字
    bzero(&serv_addr, sizeof(serv_addr));
    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(SERV_PORT); // 主机转网络字节序
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr.s_addr); // 点分十进制转网络字节序
    bind(lfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    listen(lfd, 128);        // 最大同时连接数
    socklen_t clie_addr_len; // 套接字长度
    printf("Accepting client connect ...\n");
    while (1) {
        clie_addr_len = sizeof(clie_addr);
        cfd = accept(lfd, (struct sockaddr*) &clie_addr, &clie_addr_len);
        if (cfd == -1) {
            perror("accept error!");
            exit(1);
        }
        ts[i].cliaddr = clie_addr;
        ts[i].cfd     = cfd;
        // 多线程
        pthread_create(&tid, NULL, do_work, (void*) &ts[i]);
        pthread_detach(tid); // 线程分离 自动回收 防止僵尸进程
        i++;
    }
    return 0;
}
