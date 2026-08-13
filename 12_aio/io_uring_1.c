#include <fcntl.h>
#include <liburing.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <unistd.h>

#define QUEUE_DEPTH 1
#define BLOCK_SIZE 4096

void io_uring_read_example(const char* filename) {
    struct io_uring      ring;
    struct io_uring_cqe* cqe;
    struct io_uring_sqe* sqe;
    char*                buffer;
    int                  fd, ret;

    // Allocate buffer for reading data
    buffer = malloc(BLOCK_SIZE);
    if (!buffer) {
        perror("malloc");
        return;
    }

    // Initialize io_uring
    ret = io_uring_queue_init(QUEUE_DEPTH, &ring, 0);
    if (ret) {
        perror("io_uring_queue_init");
        free(buffer);
        return;
    }

    // Open the file to read
    fd = open(filename, O_RDONLY);
    if (fd == -1) {
        perror("open");
        io_uring_queue_exit(&ring);
        free(buffer);
        return;
    }

    // Prepare the I/O request to read data
    sqe = io_uring_get_sqe(&ring);
    io_uring_prep_read(sqe, fd, buffer, BLOCK_SIZE, 0);

    // Submit the request to the io_uring queue
    ret = io_uring_submit(&ring);
    if (ret < 0) {
        perror("io_uring_submit");
        close(fd);
        io_uring_queue_exit(&ring);
        free(buffer);
        return;
    }

    // Wait for the completion event
    ret = io_uring_wait_cqe(&ring, &cqe);
    if (ret < 0) {
        perror("io_uring_wait_cqe");
        close(fd);
        io_uring_queue_exit(&ring);
        free(buffer);
        return;
    }

    // Check the result of the I/O operation
    if (cqe->res < 0) {
        fprintf(stderr, "Read failed: %s\n", strerror(-cqe->res));
    } else {
        printf("Read %d bytes: %s\n", cqe->res, buffer);
    }

    // Mark the completion event as handled
    io_uring_cqe_seen(&ring, cqe);

    // Clean up
    close(fd);
    io_uring_queue_exit(&ring);
    free(buffer);
}

int main() {
    io_uring_read_example("testfile.txt");
    return 0;
}
