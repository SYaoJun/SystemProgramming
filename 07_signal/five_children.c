#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int  numDone = 0;
void reapChildren(int status) {
    waitpid(-1, NULL, 0);
    numDone++;
}

int main() {
    int   i, status;
    pid_t pid;
    signal(SIGCHLD, reapChildren); // Set signal handler

    for (i = 0; i < 5; i++) {
        pid = fork();

        if (pid == 0) {
            // Child process
            printf("Child process %d starting...\n", getpid());
            // Child process work...
            sleep(2 * i); // Assume child process does 2 seconds of work
            printf("Child process %d exiting...\n", getpid());
            exit(EXIT_SUCCESS);
        }
    }

    while (numDone < 5) {
        sleep(2);
        printf("Child process exited with status: %d\n", status);
    }

    printf("All child processes have completed. Parent process exiting...\n");
    return 0;
}
