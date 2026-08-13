#include <errno.h>
#include <fcntl.h>
#include <getopt.h>
#include <signal.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
#define PIPE_READ_AND_WRITE 2
#define PIPE_READ 0
#define PIPE_WRITE 1
#define THREAD_NUM 3
void A(int pipeAB[PIPE_READ_AND_WRITE], int pipeAC[PIPE_READ_AND_WRITE]) {
    close(pipeAB[PIPE_READ]);
    close(pipeAC[PIPE_READ]);
    // 把标准输出设置为管道AB的写端
    dup2(pipeAB[PIPE_WRITE], STDOUT_FILENO);
    // 把标准错误设置为管道AC的写端
    dup2(pipeAC[PIPE_WRITE], STDERR_FILENO);
    close(pipeAB[PIPE_WRITE]);
    close(pipeAC[PIPE_WRITE]);
    fprintf(stderr, "on process A, write info into stderr.\n");
    fprintf(stdout, "on process A, write info into stdout.\n");
    fflush(stderr);
    fflush(stdout);
    exit(0);
}

void B(int pipeAB[PIPE_READ_AND_WRITE], int pipeAC[PIPE_READ_AND_WRITE]) {
    close(pipeAB[PIPE_WRITE]);
    close(pipeAC[PIPE_READ]);
    close(pipeAC[PIPE_WRITE]);
    // 把标准输入设置为从AB的读端读入数据，把AB读端复制到标准输入，操作标准输入就是操作AB读端
    dup2(pipeAB[PIPE_READ], STDIN_FILENO);
    close(pipeAB[PIPE_READ]);

    char buf[1024];
    memset(buf, 0, sizeof(buf));
    read(STDIN_FILENO, buf, 1024);
    printf("stdout = %s", buf);
    exit(0);
}
void C(int pipeAB[PIPE_READ_AND_WRITE], int pipeAC[PIPE_READ_AND_WRITE]) {
    close(pipeAB[PIPE_READ]);
    close(pipeAB[PIPE_WRITE]);
    close(pipeAC[PIPE_WRITE]);
    // 把标准输入设置为从AC的读端读入数据
    dup2(pipeAC[PIPE_READ], STDIN_FILENO);
    close(pipeAC[PIPE_READ]);
    char buf[1024];
    memset(buf, 0, sizeof(buf));
    read(STDIN_FILENO, buf, 1024);
    printf("stderr = %s", buf);
    exit(0);
}
int main() {
    int pipeAB[PIPE_READ_AND_WRITE];
    int pipeAC[PIPE_READ_AND_WRITE];
    int children[THREAD_NUM];
    pipe(pipeAB);
    pipe(pipeAC);
    pid_t pid;

    for (int i = 0; i < THREAD_NUM; i++) {
        pid = fork();
        if (pid == 0) {
            if (i == 0) {
                A(pipeAB, pipeAC);
            } else if (i == 1) {
                B(pipeAB, pipeAC);
            } else {
                C(pipeAB, pipeAC);
            }
        } else {
            children[i] = pid;
        }
    }

    int status;
    for (int i = 0; i < THREAD_NUM; i++) {
        waitpid(children[i], &status, 0);
    }
    return 0;
}
