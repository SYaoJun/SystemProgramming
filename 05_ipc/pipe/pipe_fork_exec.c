#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
#include <unistd.h>

#define READ_END 0
#define WRITE_END 1

int main() {
    int   stdout_pipe[2];
    int   stderr_pipe[2];
    pid_t pid;

    // Create pipes
    if (pipe(stdout_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }
    if (pipe(stderr_pipe) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    pid = fork();
    if (pid == -1) {
        perror("fork");
        exit(EXIT_FAILURE);
    }

    if (pid == 0) { // Child process
        // Redirect stdout to stdout_pipe
        close(stdout_pipe[READ_END]);
        dup2(stdout_pipe[WRITE_END], STDOUT_FILENO);
        close(stdout_pipe[WRITE_END]);

        // Redirect stderr to stderr_pipe
        close(stderr_pipe[READ_END]);
        dup2(stderr_pipe[WRITE_END], STDERR_FILENO);
        close(stderr_pipe[WRITE_END]);

        // Execute command
        execlp("your_command", "your_command", "arg1", "arg2", (char*) NULL);
        perror("execlp"); // If execlp fails
        exit(EXIT_FAILURE);
    } else { // Parent process
        close(stdout_pipe[WRITE_END]);
        close(stderr_pipe[WRITE_END]);

        // Read stdout
        char    buffer[128];
        ssize_t count;
        while ((count = read(stdout_pipe[READ_END], buffer, sizeof(buffer) - 1))
            > 0) {
            buffer[count] = '\0';
            printf("Standard Output: %s", buffer);
        }

        // Read stderr
        while ((count = read(stderr_pipe[READ_END], buffer, sizeof(buffer) - 1))
            > 0) {
            buffer[count] = '\0';
            fprintf(stderr, "Standard Error: %s", buffer);
        }

        close(stdout_pipe[READ_END]);
        close(stderr_pipe[READ_END]);

        // Wait for child process to exit
        int status;
        waitpid(pid, &status, 0);
        if (WIFEXITED(status)) {
            printf(
                "Child process exited with status %d\n", WEXITSTATUS(status));
        } else {
            printf("Child process did not exit successfully\n");
        }
    }

    return 0;
}
