#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

int main() {
    // open write read
    // 功能：从一个文件读内容，写到另一个文件
    int fd = open("hello.txt", O_RDONLY);
    if (fd == -1) {
        printf("open failed!\n");
        return 0;
    }
    int  fx = open("newhello.txt", O_CREAT | O_WRONLY, 0644);
    char buf[1024];
    int  n;
    while (n = read(fd, buf, sizeof(buf))) {
        int ret = write(fx, buf, n);
        if (ret == -1) {
            perror("write error");
            exit(1);
        }
        printf("write bytes: %d\n", n);
    }

    close(fd);
    close(fx);
    return 0;
}
