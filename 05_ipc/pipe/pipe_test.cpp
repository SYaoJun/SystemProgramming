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
    // Parent writes to fd[1]
    // Child reads from fd[0]
    int   fd[2]; // 0 is read end (child), 1 is write end (parent)
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
        // Child process, read from fd[0]
        close(fd[1]);
        char buf[1024];
        printf("child = %d\n", getpid());
        ret = read(fd[0], buf, sizeof(buf));
        write(STDOUT_FILENO, buf, ret);
    } else if (pid > 0) {
        // Parent process, write to fd[1]
        sleep(1);
        close(fd[0]);
        printf("parent = %d\n", getpid());
        write(fd[1], "hello pipe\n", 11);
        wait(NULL); // Reap child process to avoid zombie process
    }
    printf("finished! %d\n", getpid());
    return 0;
}
