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
1. Create socket
2. Call connect() to connect to the server
3. Write data from keyboard
4. Read data from server
5.close
*/
int main(void) {
    // 1.
    int                cfd;
    struct sockaddr_in serv_addr;
    socklen_t          serv_addr_len;
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    // 2.
    memset(&serv_addr, 0, sizeof(serv_addr)); // initialize
    serv_addr.sin_family = AF_INET;           // IPv4 protocol
    serv_addr.sin_port   = htons(SERV_PORT);  // server port
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr.s_addr); // dotted decimal to network byte order
    // 3.
    connect(cfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));
    // 4.
    char buf[BUFSIZ];
    int  n;
    while (1) {
        fgets(buf, sizeof(buf),
            stdin); // read a line, but a newline is automatically appended
        write(cfd, buf, strlen(buf)); // write to file descriptor, write buffer
        n = read(cfd, buf,
            sizeof(buf)); // returns the number of bytes read from the server
        write(STDOUT_FILENO, buf, n); // write the read data to the screen
    }
    // 5.
    close(cfd);
    return 0;
}
