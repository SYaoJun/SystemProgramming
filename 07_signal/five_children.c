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
    signal(SIGCHLD, reapChildren); // 设置信号处理函数

    for (i = 0; i < 5; i++) {
        pid = fork();

        if (pid == 0) {
            // 子进程
            printf("Child process %d starting...\n", getpid());
            // 子进程的工作...
            sleep(2 * i); // 假设子进程执行了2秒钟的工作
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
