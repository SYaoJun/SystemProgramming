#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
int my_alarm(int sec) {
    struct itimerval new_it, old_it;
    // new_it: input
    // old_it: output
    new_it.it_value.tv_sec  = sec; // Timer duration
    new_it.it_value.tv_usec = 0;   // Microseconds

    new_it.it_interval.tv_sec  = 0; // Periodic timer interval
    new_it.it_interval.tv_usec = 0;
    // argv_1: real-time timer; also supports user-mode or user+kernel mode
    // timing
    int ret = setitimer(ITIMER_REAL, &new_it, &old_it);
    if (ret == -1) {
        perror("setitimer error");
        exit(1);
    }
    return old_it.it_value.tv_sec;
}
int main() {
    my_alarm(1); // After 1s timeout, kernel sends SIGALRM to terminate
    for (int i = 0;; i++)
        printf("%d\n", i);
    return 0;
}
