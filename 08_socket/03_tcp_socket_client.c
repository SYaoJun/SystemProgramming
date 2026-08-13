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
1. Create socket()
2. Call connect() to connect to the server
3. Input data from keyboard
4. Read data from server
5.close
*/
int main(void) {
    // 1.
    int                cfd;
    struct sockaddr_in serv_addr;
    socklen_t          serv_addr_len;
    // socket: input buffer / output buffer
    // File descriptor: an integer that serves as an identifier for accessing a
    // buffer
    cfd = socket(AF_INET, SOCK_STREAM, 0);
    // Initialize socket parameters
    memset(&serv_addr, 0, sizeof(serv_addr)); // Initialize
    serv_addr.sin_family = AF_INET;           // IPv4 protocol
    serv_addr.sin_port   = htons(SERV_PORT);  // Server port
    inet_pton(AF_INET, SERV_IP,
        &serv_addr.sin_addr
            .s_addr); // Convert dotted-decimal to network byte order
    // 2.
    //  The server IP+PORT must be specified; the client IP and port can be
    //  randomly chosen.
    int ret = connect(cfd, (struct sockaddr*) &serv_addr, sizeof(serv_addr));

    char buf[BUFSIZ];
    int  n;
    // while (1) {
    // 3/4.
    printf("please input a string: ");
    fgets(buf, sizeof(buf), stdin); // Read one line, but a newline character is
                                    // automatically appended
    // Remove the newline character
    int len = strlen(buf);
    if (len >= 1 && buf[len - 1] == '\n') {
        buf[len - 1] = '\0';
    }
    write(cfd, buf, strlen(buf)); // Write to the file descriptor (write buffer)
    // n = read(cfd, buf, sizeof(buf));  // Returns the number of bytes read
    // from the server write(STDOUT_FILENO, buf, n);     // Write the read data
    // to the screen
    // }
    // 5.
    shutdown(cfd, SHUT_WR);
    sleep(100);
    return 0;
}
