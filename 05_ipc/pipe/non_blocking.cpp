#include <fcntl.h>
#include <stdio.h>
#include <unistd.h>

int setnonblocking(int fd) {
    int old_option = fcntl(fd, F_GETFL);
    int new_option = old_option | O_NONBLOCK;
    fcntl(fd, F_SETFL, new_option);
    return old_option;
}
int main() {
    int fd = open("jun.txt", O_CREAT | O_RDWR);
    if (fd < 0) {
        printf("error!");
        _exit(0);
    }
    int old_ = setnonblocking(fd);
    int new_ = fcntl(fd, F_GETFL);
    printf("old = %d, new = %d\n", old_, new_);
    return 0;
}
