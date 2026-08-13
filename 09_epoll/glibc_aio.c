#include <aio.h>
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

void aio_completion_handler(sigval_t sigval);

int main() {
    struct aiocb aio_req;
    int          fd;
    char*        buffer;

    // Open the file for reading
    fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Allocate buffer
    buffer = malloc(1024);
    if (!buffer) {
        perror("malloc");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Initialize the aiocb structure
    memset(&aio_req, 0, sizeof(struct aiocb));
    aio_req.aio_fildes = fd;
    aio_req.aio_buf    = buffer;
    aio_req.aio_nbytes = 1024;
    aio_req.aio_offset = 0;

    // Set up the callback function
    aio_req.aio_sigevent.sigev_notify            = SIGEV_THREAD;
    aio_req.aio_sigevent.sigev_notify_function   = aio_completion_handler;
    aio_req.aio_sigevent.sigev_notify_attributes = NULL;
    aio_req.aio_sigevent.sigev_value.sival_ptr   = &aio_req;

    // Start the read operation
    if (aio_read(&aio_req) == -1) {
        perror("aio_read");
        free(buffer);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Wait for the read operation to complete
    while (aio_error(&aio_req) == EINPROGRESS) {
        usleep(1000);
    }

    // Check the status of the read operation
    if (aio_return(&aio_req) == -1) {
        perror("aio_return");
        free(buffer);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Clean up
    free(buffer);
    close(fd);

    return 0;
}

void aio_completion_handler(sigval_t sigval) {
    struct aiocb* req = (struct aiocb*) sigval.sival_ptr;

    if (aio_error(req) == 0) {
        int bytes_read = aio_return(req);
        printf("Read %d bytes: %s\n", bytes_read, (char*) req->aio_buf);
    } else {
        perror("aio_error");
    }
}

#include <signal.h>

void signal_handler(int signo, siginfo_t* info, void* context) {
    struct aiocb* req = (struct aiocb*) info->si_value.sival_ptr;

    if (aio_error(req) == 0) {
        int bytes_read = aio_return(req);
        printf("Read %d bytes: %s\n", bytes_read, (char*) req->aio_buf);
    } else {
        perror("aio_error");
    }
}

int main() {
    struct aiocb     aio_req;
    int              fd;
    char*            buffer;
    struct sigaction sa;

    // Open the file for reading
    fd = open("example.txt", O_RDONLY);
    if (fd == -1) {
        perror("open");
        exit(EXIT_FAILURE);
    }

    // Allocate buffer
    buffer = malloc(1024);
    if (!buffer) {
        perror("malloc");
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Initialize the aiocb structure
    memset(&aio_req, 0, sizeof(struct aiocb));
    aio_req.aio_fildes = fd;
    aio_req.aio_buf    = buffer;
    aio_req.aio_nbytes = 1024;
    aio_req.aio_offset = 0;

    // Set up the signal handler
    sa.sa_flags     = SA_SIGINFO;
    sa.sa_sigaction = signal_handler;
    sigemptyset(&sa.sa_mask);
    if (sigaction(SIGUSR1, &sa, NULL) == -1) {
        perror("sigaction");
        free(buffer);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Set up the sigevent structure
    aio_req.aio_sigevent.sigev_notify          = SIGEV_SIGNAL;
    aio_req.aio_sigevent.sigev_signo           = SIGUSR1;
    aio_req.aio_sigevent.sigev_value.sival_ptr = &aio_req;

    // Start the read operation
    if (aio_read(&aio_req) == -1) {
        perror("aio_read");
        free(buffer);
        close(fd);
        exit(EXIT_FAILURE);
    }

    // Wait for the read operation to complete
    while (aio_error(&aio_req) == EINPROGRESS) {
        pause();
    }

    // Clean up
    free(buffer);
    close(fd);

    return 0;
}
