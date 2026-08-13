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

    // 父进程阻塞 SIGINT 信号
    if (sigprocmask(SIG_BLOCK, &newmask, &oldmask) < 0) {
        perror("sigprocmask");
        return 1;
    }

    pid_t pid = fork();
    if (pid == 0) {
        // 子进程
        struct sigaction sa;
        sa.sa_handler = handle_sigint;
        sigemptyset(&sa.sa_mask);
        sa.sa_flags = 0;

        // 子进程解除对 SIGINT 的阻塞，并设置处理函数
        if (sigaction(SIGINT, &sa, NULL) < 0) {
            perror("sigaction");
            return 1;
        }

        while (1) {
            // 子进程的执行逻辑
        }
    } else if (pid > 0) {
        // 父进程
        printf("Parent process\n");
        sleep(5);

        // 父进程恢复原来的信号掩码
        if (sigprocmask(SIG_SETMASK, &oldmask, NULL) < 0) {
            perror("sigprocmask");
            return 1;
        }
    } else {
        // fork 失败
        perror("fork");
        return 1;
    }

    return 0;
}
