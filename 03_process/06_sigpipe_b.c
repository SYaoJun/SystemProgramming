#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/wait.h>
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

    pid_t pid = fork();
    if (pid == 0) {
        // child
        // 子读0 父写1
        close(pipefd[1]);
        char buf[1024];
        read(pipefd[0], buf, sizeof(buf));
        printf("buf:%s\n", buf);
        close(pipefd[0]);
        exit(0);
    } else {
        // parent
        char* s = "hello";
        close(pipefd[0]);
        write(pipefd[1], s, 5);
        printf("close\n");
        fflush(stdout);
        int status = 0;
        waitpid(pid, &status, 0);
        write(pipefd[1], s, 5);
        // write message to closed pipe will occur SIGPIPE
    }
    return 0;
}
