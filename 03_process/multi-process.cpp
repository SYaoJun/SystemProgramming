//
// Created by Yao Jun on 2022/8/13.
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
    // 5 processes computing in parallel
    int  THREAD_NUM = 5;
    LL   total_sum  = 0;
    auto start      = std::chrono::steady_clock::now();
    int  i;
    for (i = 0; i < THREAD_NUM; i++) {
        pid_t pid = fork(); // n  2^n-1  n
        if (pid == 0) {
            // Variables are not visible between processes, how to fix this?
            total_sum += get_interval_sum(i);
            break;
        }
    }
    if (i < THREAD_NUM) { // Child process exits
        // printf("I'm %dth child, pid = %u, ppid = %u\n", i + 1, getpid(),
        // getppid());
    } else { // Parent process exits
        // wait(NULL); // NULL: do not care about child process exit status
        // waitpid reclaims a specified process
        // argv_1: pid specifies child process, -1 for any child process
        // argv_2: child process exit status, output parameter.
        // argv_3:
        // WNOHANG: non-blocking, reclaim by polling; returns child pid on
        // success, 0 if not yet exited, -1 on error
        int reclaim_num = 0;
        do {
            auto wpid = waitpid(-1, NULL, WNOHANG);
            if (wpid > 0) {
                reclaim_num++;
            }
        } while (reclaim_num != THREAD_NUM);

        printf("I'm parent, pid = %u, ppid = %u\n", getpid(), getppid());
        printf("multi-process result = %lld\n", total_sum);
        // Parent process waits for child processes to finish before exiting
        auto end = std::chrono::steady_clock::now();
        auto duration_ms
            = std::chrono::duration_cast<std::chrono::microseconds>(end - start)
                  .count();
        printf("多进程花费了%lld毫秒\n", duration_ms);
    }

    return 0;
}
