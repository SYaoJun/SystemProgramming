#include <stdio.h>
#include <unistd.h>
int main() {
    alarm(3); // 计时1s 到时后内核发送一个SIGALRM信号终止进程, 只能定时秒级
    long long i = 0;
    while (true) {
        i++;
        printf("processing...%lld\n", i);
    }
    return 0;
}
