#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
void process_func(int x) {
    printf("I catch a signal. ha ha ha...\n");
}
int main() {
    // 信号注册
    signal(SIGALRM, process_func);
    struct itimerval new_it, old_it;
    // new_id传入
    // old_it传出
    // 定时为：5秒后开始，间隔3秒一次
    new_it.it_value.tv_sec  = 1; // 定时长度
    new_it.it_value.tv_usec = 0; // 微秒

    new_it.it_interval.tv_sec  = 3; // 周期定时
    new_it.it_interval.tv_usec = 0;
    // argv_1：自然定时，也可以支持用户态时间或者用户态和内核态计时
    if (setitimer(ITIMER_REAL, &new_it, &old_it) == -1) {
        perror("setitimer error");
        exit(1);
    }
    while (1)
        ;
    return 0;
}
