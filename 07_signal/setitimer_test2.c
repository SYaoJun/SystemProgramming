#include <signal.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <unistd.h>
void process_func(int x) {
    printf("I catch a signal. ha ha ha...\n");
}
int main() {
    // Signal registration
    signal(SIGALRM, process_func);
    struct itimerval new_it, old_it;
    // new_it: input
    // old_it: output
    // Timer: starts after 5 seconds, repeats every 3 seconds
    new_it.it_value.tv_sec  = 1; // Timer duration
    new_it.it_value.tv_usec = 0; // Microseconds

    new_it.it_interval.tv_sec  = 3; // Periodic timer interval
    new_it.it_interval.tv_usec = 0;
    // argv_1: real-time timer; also supports user-mode or user+kernel mode
    // timing
    if (setitimer(ITIMER_REAL, &new_it, &old_it) == -1) {
        perror("setitimer error");
        exit(1);
    }
    while (1)
        ;
    return 0;
}
