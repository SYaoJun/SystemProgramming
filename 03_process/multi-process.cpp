//
// Created by 姚军 on 2022/8/13.
//

#include <chrono>
#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <unistd.h>

typedef long long LL;
LL                get_interval_sum(int start) {
    start *= 20000;
    int stop = start + 20000;
    LL  temp = 0;
    for (int i = start; i < stop; i++) {
        temp += i * (LL) i;
    }
    printf("I'm %dth, temp = %lld\n", start / 20000, temp);
    return temp;
}
int main() {
    // 5个进程并行计算
    int  THREAD_NUM = 5;
    LL   total_sum  = 0;
    auto start      = std::chrono::steady_clock::now();
    int  i;
    for (i = 0; i < THREAD_NUM; i++) {
        pid_t pid = fork(); // n  2^n-1  n
        if (pid == 0) {
            // 进程之间变量不可见，怎么改呢？
            total_sum += get_interval_sum(i);
            break;
        }
    }
    if (i < THREAD_NUM) { // 子进程退出
        // printf("I'm %dth child, pid = %u, ppid = %u\n", i + 1, getpid(),
        // getppid());
    } else { // 父进程退出
        // wait(NULL); //NULL不关心子进程退出的状态
        // waitpid回收指定进程
        // argv_1: pid指定子进程，-1任意子进程
        // argv_2: 子进程退出的状态，出参。
        // argv_3:
        // WNOHANG不阻塞，通过轮询回收，回收成功返回子进程pid，回收失败返回0，出错返回-1
        int reclaim_num = 0;
        do {
            auto wpid = waitpid(-1, NULL, WNOHANG);
            if (wpid > 0) {
                reclaim_num++;
            }
        } while (reclaim_num != THREAD_NUM);

        printf("I'm parent, pid = %u, ppid = %u\n", getpid(), getppid());
        printf("multi-process result = %lld\n", total_sum);
        // 父进程等待子进程执行完毕再结束
        auto end = std::chrono::steady_clock::now();
        auto duration_ms
            = std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                  .count();
        printf("多进程花费了%lld毫秒\n", duration_ms);
    }

    return 0;
}
