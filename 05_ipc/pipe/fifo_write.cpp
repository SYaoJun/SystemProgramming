#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>
#define P_FIFO "/tmp/p_fifo"
int main(int argc, char** argv) {
    int fd;
    if (argc < 2) {
        printf("please input the write data.\n");
        exit(1);
    }
    fd = open(P_FIFO, O_WRONLY | O_NONBLOCK); /*非阻塞方式*/
    if (fd == -1) {
        printf("open failed!\n");
        return 0;
    } else {
        printf("open success!");
    }
    write(fd, argv[1], 100); /*将argv[1]写道fd里面去*/
    close(fd);
    return 0;
}
