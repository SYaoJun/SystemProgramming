#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int main() {
    // 父亲写 1
    // 孩子读 0
    int   fd[2]; // 0为读(子) 1为写(父)
    pid_t pid;
    int   ret = pipe(fd);
    if (ret == -1) {
        perror("pipe error!");
        exit(1);
    }
    pid = fork();
    if (pid == -1) {
        perror("fork error!");
        exit(1);
    } else if (pid == 0) {
        // 子进程 读0
        close(fd[1]);
        char buf[1024];
        printf("child = %d\n", getpid());
        ret = read(fd[0], buf, sizeof(buf));
        write(STDOUT_FILENO, buf, ret);
    } else if (pid > 0) {
        // 父进程 写1
        sleep(1);
        close(fd[0]);
        printf("parent = %d\n", getpid());
        write(fd[1], "hello pipe\n", 11);
        wait(NULL); // 回收子进程，避免成为僵尸进程
    }
    printf("finished! %d\n", getpid());
    return 0;
}
