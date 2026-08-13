#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

int main() {
    int fd = open("/tmp/testfile", O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Set the file descriptor to non-blocking mode
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1) {
        perror("fcntl F_GETFL");
        close(fd);
        exit(EXIT_FAILURE);
    }

    flags |= O_NONBLOCK;
    if (fcntl(fd, F_SETFL, flags) == -1) {
        perror("fcntl F_SETFL");
        close(fd);
        exit(EXIT_FAILURE);
    }

    char    buffer[128];
    ssize_t bytesRead;

    while (1) {
        bytesRead = read(fd, buffer, sizeof(buffer) - 1);
        if (bytesRead == -1) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                // No data available, try again later
                printf("No data available, try again later\n");
                sleep(1);
                continue;
            } else {
                perror("read");
                close(fd);
                exit(EXIT_FAILURE);
            }
        } else if (bytesRead == 0) {
            // End of file
            printf("End of file\n");
            break;
        }

        buffer[bytesRead] = '\0';
        printf("Read: %s\n", buffer);
    }

    close(fd);
    return 0;
}
