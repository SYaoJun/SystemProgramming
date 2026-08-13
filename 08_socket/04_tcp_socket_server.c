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
    char               buf[BUFSIZ], client_IP[BUFSIZ]; // 4k or 8k
    struct sockaddr_in serv_addr, clie_addr;
    // socket is an input and output buffer
    // 1.
    // lfd is used for listening only, not for reading/writing actual data;
    // socket file descriptor: 37
    lfd = socket(AF_INET, SOCK_STREAM, 0); // Create socket

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port   = htons(server_port);
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr
            .s_addr); // Convert dotted-decimal to network byte order

    // Bind port — like booking classroom A5 for a class; clients are students
    // who need to know classroom A5 Bind the service interface 2.
    bind(lfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    // Listen on port
    // 3.
    listen(lfd, 128);        // Maximum simultaneous connections
    socklen_t clie_addr_len; // Socket address length
    clie_addr_len = sizeof(clie_addr);
    // Accept request — blocking call, waits
    // 4.
    cfd = accept(lfd, (struct sockaddr*) &clie_addr, &clie_addr_len);
    // The returned cfd is the one actually used for reading/writing data
    printf("client IP: %s, client port: %d\n",
        inet_ntop(
            AF_INET, &clie_addr.sin_addr.s_addr, client_IP, sizeof(client_IP)),
        ntohs(clie_addr.sin_port));
    int i, n;
    // Run "nc 127.0.0.1 6670" in terminal to send data
    // How the server supports RESTful-style requests: GET, POST, PUT, DELETE
    while (1) { // Keep reading and writing
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
    // All file descriptors need to be closed
    close(lfd);
    close(cfd);
    return 0;
}
