#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h>
#include <sys/wait.h>
#include <unistd.h>
int  numDone = 0;
void reapChildren(int status) {
    puts("receive signal");
    waitpid(-1, NULL, 0);
    numDone++;
}

int main() {
    int   i, status;
    pid_t pid;
    signal(SIGCHLD, reapChildren); // 设置信号处理函数
    puts("do something!");
    // raise(SIGCHLD);
    kill(getpid(), SIGCHLD);
    printf("numDone = %d\n", numDone);
    return 0;
}
