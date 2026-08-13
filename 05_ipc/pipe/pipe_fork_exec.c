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

    // 创建管道
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

    if (pid == 0) { // 子进程
        // 重定向标准输出到 stdout_pipe
        close(stdout_pipe[READ_END]);
        dup2(stdout_pipe[WRITE_END], STDOUT_FILENO);
        close(stdout_pipe[WRITE_END]);

        // 重定向标准错误到 stderr_pipe
        close(stderr_pipe[READ_END]);
        dup2(stderr_pipe[WRITE_END], STDERR_FILENO);
        close(stderr_pipe[WRITE_END]);

        // 执行命令
        execlp("your_command", "your_command", "arg1", "arg2", (char*) NULL);
        perror("execlp"); // 如果execlp失败
        exit(EXIT_FAILURE);
    } else { // 父进程
        close(stdout_pipe[WRITE_END]);
        close(stderr_pipe[WRITE_END]);

        // 读取标准输出
        char    buffer[128];
        ssize_t count;
        while ((count = read(stdout_pipe[READ_END], buffer, sizeof(buffer) - 1))
            > 0) {
            buffer[count] = '\0';
            printf("Standard Output: %s", buffer);
        }

        // 读取标准错误
        while ((count = read(stderr_pipe[READ_END], buffer, sizeof(buffer) - 1))
            > 0) {
            buffer[count] = '\0';
            fprintf(stderr, "Standard Error: %s", buffer);
        }

        close(stdout_pipe[READ_END]);
        close(stderr_pipe[READ_END]);

        // 等待子进程结束
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
