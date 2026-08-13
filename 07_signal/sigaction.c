#include <signal.h>
#include <stdio.h>
#include <unistd.h>

void sig_usr(int signum) {
    if (signum == SIGUSR1) {
        printf("Received SIGUSR1\n");
    } else if (signum == SIGUSR2) {
        printf("Received SIGUSR2\n");
    } else {
        printf("Received signal %d\n", signum);
    }
}

int main() {
    struct sigaction sa_usr;

    sa_usr.sa_flags   = 0;
    sa_usr.sa_handler = sig_usr; // 设置信号处理函数

    // 为 SIGUSR1 信号设置处理动作
    if (sigaction(SIGUSR1, &sa_usr, NULL) == -1) {
        perror("sigaction for SIGUSR1 failed");
        return 1;
    }

    // 为 SIGUSR2 信号设置处理动作
    if (sigaction(SIGUSR2, &sa_usr, NULL) == -1) {
        perror("sigaction for SIGUSR2 failed");
        return 1;
    }

    printf("My PID is %d\n", getpid());

    while (1) {
        // 这里可以进行其他的程序逻辑
        sleep(1);
    }

    return 0;
}
