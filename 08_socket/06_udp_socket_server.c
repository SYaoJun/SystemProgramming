#include <netinet/in.h>
#include <stdio.h>
#include <string.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <unistd.h>
#define SERVER_PORT 8888
#define BUFF_LEN 1024

void handle_udp_msg(int fd) {
    char      buf[BUFF_LEN]; // Receive buffer, 1024 bytes
    socklen_t len;
    int       count;
    struct sockaddr_in
        clent_addr; // clent_addr records the sender address information
    while (1) {
        memset(buf, 0, BUFF_LEN);
        len   = sizeof(clent_addr);
        count = recvfrom(fd, buf, BUFF_LEN, 0, (struct sockaddr*) &clent_addr,
            &len); // recvfrom is a blocking function; it blocks when there is
                   // no data
        if (count == -1) {
            printf("recieve data fail!\n");
            return;
        }
        printf("client:%s\n", buf); // Print the message received from client
        memset(buf, 0, BUFF_LEN);
        sprintf(
            buf, "I have recieved %d bytes data!\n", count); // Reply to client
        printf("server:%s\n", buf); // Print the message sent by self
        sendto(fd, buf, BUFF_LEN, 0, (struct sockaddr*) &clent_addr,
            len); // Send message to client; note the use of clent_addr struct
                  // pointer
    }
}

/*
    server:
            socket-->bind-->recvfrom-->sendto-->close
*/

int main(int argc, char* argv[]) {
    int                server_fd, ret;
    struct sockaddr_in ser_addr;

    server_fd = socket(AF_INET, SOCK_DGRAM, 0); // AF_INET:IPV4;SOCK_DGRAM:UDP
    if (server_fd < 0) {
        printf("create socket fail!\n");
        return -1;
    }

    memset(&ser_addr, 0, sizeof(ser_addr));
    ser_addr.sin_family = AF_INET;
    ser_addr.sin_addr.s_addr
        = htonl(INADDR_ANY); // IP address, needs network byte order conversion;
                             // INADDR_ANY: local address
    ser_addr.sin_port = htons(
        SERVER_PORT); // Port number, needs network byte order conversion

    ret = bind(server_fd, (struct sockaddr*) &ser_addr, sizeof(ser_addr));
    if (ret < 0) {
        printf("socket bind fail!\n");
        return -1;
    }

    handle_udp_msg(server_fd); // Handle received data

    close(server_fd);
    return 0;
}
