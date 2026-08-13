#include <arpa/inet.h>
#include <ctype.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <unistd.h>

#define SERV_IP "127.0.0.1"

#define MAX_CONNECTION 20
int get_ephemeral_port(int server_port, int lfd) {
    if (!server_port) {
        struct sockaddr_in ad;
        memset(&ad, 0, sizeof(struct sockaddr_in));
        socklen_t len = sizeof(struct sockaddr_in);
        // 5. Get ephemeral port, returns the address bound to this fd; the last
        // two parameters are output params
        if (getsockname(lfd, (struct sockaddr*) &ad, &len) == -1) {
            perror("getsockname: ");
            exit(EXIT_FAILURE);
        } else {
            // network to host
            return ntohs(ad.sin_port);
        }
    }
    return server_port;
}
/*
usage:
    ./server [port]
*/
int main(int argc, char* argv[]) {
    int server_port = 0;
    if (argc >= 2) {
        server_port = atoi(argv[0]);
    }
    // 1. Create a socket
    int lfd = socket(AF_INET, SOCK_STREAM, 0);
    if (lfd == -1) {
        perror("create socket");
        exit(EXIT_FAILURE);
    }
    // 2. Port reuse
    int reuse = 1;
    setsockopt(lfd, SOL_SOCKET, SO_REUSEADDR, &reuse, sizeof(reuse));
    struct sockaddr_in serverAddr;
    serverAddr.sin_family = AF_INET;
    // If port=0, use an ephemeral port; the system randomly assigns an unused
    // port
    /*
        If this is zero or this argument pair is absent, then uqchessserver is
       to use an ephemeral port. host to network
    */
    serverAddr.sin_port = htons(server_port);
    // Client requests from any local IP
    serverAddr.sin_addr.s_addr = INADDR_ANY;
    // 3. Bind IP
    if (bind(lfd, (struct sockaddr*) &serverAddr, sizeof(struct sockaddr))
        == -1) {
        perror("bind: ");
        exit(EXIT_FAILURE);
    }
    // 4. Set listen backlog
    if (listen(lfd, MAX_CONNECTION) == -1) {
        perror("listen: ");
        exit(EXIT_FAILURE);
    }
    int real_port = get_ephemeral_port(server_port, lfd);
    printf("port = %d\n", real_port);
    return 0;
}
