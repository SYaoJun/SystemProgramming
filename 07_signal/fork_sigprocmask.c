#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void handle_sigint(int signum) {
    printf("Received SIGINT in child\n");
}

int main() {
    sigset_t newmask, oldmask;
    sigemptyset(&newmask);
    sigaddset(&newmask, SIGINT);

    // Parent process blocks SIGINT
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        perror("sigprocmask");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // Child process
        struct sigaction sa;
        sa.sa_handler = handle_sigint;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        // Child process unblocks SIGINT and sets handler
        if (sigaction(SIGINT, &sa, NULL) < 0) {
            perror("sigaction");
            return 1;
        }

        while (1) {
            // Child process execution logic
        }
    } else if (pid > 0) {
        // Parent process
        printf("Parent process\n");
        sleep(5);

        // Parent process restores original signal mask
        if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
            perror("sigprocmask");
            return 1;
        }
    } else {
        // fork failed
        perror("fork");
        return 1;
    }

    return 0;
}
