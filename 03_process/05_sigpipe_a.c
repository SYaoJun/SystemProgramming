#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigpipe_handler(int signum) {
    printf("Caught SIGPIPE signal: %d\n", signum);
    exit(1); // Exit the program
}

int main() {
    int  pipefd[2];
    char buffer[] = "Hello, World!";

    // Create pipe
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // Install SIGPIPE signal handler
    signal(SIGPIPE, sigpipe_handler);

    // Close the read end of the pipe
    close(pipefd[0]);

    // Write data to the pipe, triggering SIGPIPE signal
    if (write(pipefd[1], buffer, sizeof(buffer)) == -1) {
        perror("write");
    }

    // Close the write end of the pipe
    close(pipefd[1]);

    return 0;
}
