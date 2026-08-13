#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
int main() {
    // Map disk file into shared memory so the file can be accessed via pointer
    // mytest.txt must have content.
    int fd = open("mytest.txt", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open error!");
        exit(1);
    }
    // Request shared mapping
    void* p = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap error!");
        exit(1);
    }
    strcpy((char*) p, "abc");
    int ret = munmap(p, 100); // Release shared mapping
    if (ret == -1) {
        perror("munmap error!");
        exit(1);
    }
    close(fd);
    return 0;
}
