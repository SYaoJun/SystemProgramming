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
    char               buf[BUFSIZ], clie_IP[BUFSIZ]; // 4k or 8k
    struct sockaddr_in serv_addr, clie_addr;
    lfd = socket(AF_INET, SOCK_STREAM, 0); // create socket
    if (lfd == -1) {
        perror("socket error!");
        exit(1);
    }
    bzero(&serv_addr, sizeof(serv_addr));

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(SERV_PORT); // host to network byte order
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr.s_addr); // dotted decimal to network byte order

    res = bind(lfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    if (res == -1) {
        perror("bind error!");
        exit(1);
    }
    listen(lfd, 128); // maximum concurrent connections
    if (res == -1) {
        perror("listen error!");
        exit(1);
    }
    socklen_t clie_addr_len; // socket address length
    clie_addr_len = sizeof(clie_addr);
    cfd           = accept(lfd, (struct sockaddr*) &clie_addr, &clie_addr_len);
    if (cfd == -1) {
        perror("accept error!");
        exit(1);
    }
    printf("client IP: %s, client port: %d\n",
        inet_ntop(
            AF_INET, &clie_addr.sin_addr.s_addr, clie_IP, sizeof(clie_IP)),
        ntohs(clie_addr.sin_port));
    int i, n;
    while (1) { // keep reading and writing
        n = read(cfd, buf, sizeof(buf));
        for (i = 0; i < n; i++) {
            buf[i] = toupper(buf[i]);
        }
        write(cfd, buf, n);
    }
    close(lfd);
    close(cfd);
    return 0;
}
