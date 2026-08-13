#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
int my_alarm(int sec) {
    struct itimerval new_it, old_it;
    // new_id传入
    // old_it传出
    new_it.it_value.tv_sec  = sec; // 定时长度
    new_it.it_value.tv_usec = 0;   // 微秒

    new_it.it_interval.tv_sec  = 0; // 周期定时
    new_it.it_interval.tv_usec = 0;
    // argv_1：自然定时，也可以支持用户态时间或者用户态和内核态计时
    int ret = setitimer(ITIMER_REAL, &new_it, &old_it);
    if (ret == -1) {
        perror("setitimer error");
        exit(1);
    }
    return old_it.it_value.tv_sec;
}
int main() {
    my_alarm(1); // 计时1s 到时后内核发送一个sigalrm信号终止
    for (int i = 0;; i++)
        printf("%d\n", i);
    return 0;
}
