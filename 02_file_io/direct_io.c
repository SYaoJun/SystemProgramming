
#define _GNU_SOURCE /* Obtain O_DIRECT definition from <fcntl.h> */
#include <fcntl.h>
#include <libaio.h>
#include <stdint.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>

#define FILE_PATH "testfile"
#define BUF_SIZE 4096 // 通常块设备的扇区大小

int main() {
    // 打开文件，使用 O_DIRECT 标志启用 Direct I/O
    int fd = open(FILE_PATH, O_WRONLY | O_CREAT | O_DIRECT, 0666);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    // 分配未对齐的内存
    // char *buffer = (char *)malloc(BUF_SIZE);
    // 如何分配对齐的内存呢？
    // posix_memalign((void **)&buffer, BUF_SIZE, BUF_SIZE);

    //
    //     void *aligned_buffer;
    // if (posix_memalign(&aligned_buffer, BUF_SIZE, BUF_SIZE) != 0) {
    //     perror("Failed to allocate aligned buffer");
    //     close(fd);
    //     return 1;
    // }
    // 使用 posix_memalign 来分配对齐内存，确保内存对齐
    char* buffer = NULL;
    // 分配4k大小，并对齐到4k
    if (posix_memalign((void**) &buffer, BUF_SIZE, BUF_SIZE) != 0) {
        perror("Failed to allocate aligned buffer");
        close(fd);
        return 1;
    }
    // if (!buffer) {
    //     perror("Failed to allocate buffer");
    //     close(fd);
    //     return 1;
    // }

    // 填充缓冲区
    memset(buffer, 'A', BUF_SIZE);

    // 尝试写入文件
    ssize_t written = write(fd, buffer, BUF_SIZE);
    if (written < 0) {
        perror("Write failed");
    } else {
        printf("Write succeeded\n");
    }

    // 清理资源
    free(buffer);
    close(fd);

    return 0;
}
