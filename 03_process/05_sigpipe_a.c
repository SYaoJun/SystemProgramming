#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void sigpipe_handler(int signum) {
    printf("Caught SIGPIPE signal: %d\n", signum);
    exit(1); // 退出程序
}

int main() {
    int  pipefd[2];
    char buffer[] = "Hello, World!";

    // 创建管道
    if (pipe(pipefd) == -1) {
        perror("pipe");
        exit(EXIT_FAILURE);
    }

    // 安装SIGPIPE信号处理程序
    signal(SIGPIPE, sigpipe_handler);

    // 关闭管道的读端
    close(pipefd[0]);

    // 向管道写入数据，产生SIGPIPE信号
    if (write(pipefd[1], buffer, sizeof(buffer)) == -1) {
        perror("write");
    }

    // 关闭管道的写端
    close(pipefd[1]);

    return 0;
}
