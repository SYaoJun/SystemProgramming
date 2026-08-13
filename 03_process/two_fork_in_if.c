#include <stdio.h>
#include <unistd.h>
int main() {

    printf("main pid = %d, ppid = %d\n", getpid(), getppid());
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("A\n");
    // 父进程 > 0, 子进程 = 0
    if (fork() && fork()) { //  创建了两个子进程
        printf("D\n");
    }
    // 1.创建了几个子进程  2
    // 2.A最多被打印几次？3
    // 3.D最多打印几次？1
    sleep(100);
    return 0;
}
