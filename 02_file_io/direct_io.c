
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
#define BUF_SIZE 4096 // Typically the sector size of a block device

int main() {
    // Open file with O_DIRECT flag to enable Direct I/O
    int fd = open(FILE_PATH, O_WRONLY | O_CREAT | O_DIRECT, 0666);
    if (fd < 0) {
        perror("Failed to open file");
        return 1;
    }

    // Allocate unaligned memory
    // char *buffer = (char *)malloc(BUF_SIZE);
    // How to allocate aligned memory?
    // posix_memalign((void **)&buffer, BUF_SIZE, BUF_SIZE);

    //
    //     void *aligned_buffer;
    // if (posix_memalign(&aligned_buffer, BUF_SIZE, BUF_SIZE) != 0) {
    //     perror("Failed to allocate aligned buffer");
    //     close(fd);
    //     return 1;
    // }
    // Use posix_memalign to allocate aligned memory, ensuring memory alignment
    char* buffer = NULL;
    // Allocate 4K size, aligned to 4K
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

    // Fill the buffer
    memset(buffer, 'A', BUF_SIZE);

    // Attempt to write to file
    ssize_t written = write(fd, buffer, BUF_SIZE);
    if (written < 0) {
        perror("Write failed");
    } else {
        printf("Write succeeded\n");
    }

    // Clean up resources
    free(buffer);
    close(fd);

    return 0;
}
