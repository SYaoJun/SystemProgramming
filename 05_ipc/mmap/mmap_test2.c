#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <unistd.h>
int main() {
    // 把磁盘文件放入共享内存，这样可以使用指针访问磁盘文件
    // mytest.txt必须有内容。
    int fd = open("mytest.txt", O_RDWR | O_CREAT, 0644);
    if (fd < 0) {
        perror("open error!");
        exit(1);
    }
    // 申请共享映射
    void* p = mmap(NULL, 100, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (p == MAP_FAILED) {
        perror("mmap error!");
        exit(1);
    }
    strcpy((char*) p, "abc");
    int ret = munmap(p, 100); // 释放共享映射
    if (ret == -1) {
        perror("munmap error!");
        exit(1);
    }
    close(fd);
    return 0;
}
