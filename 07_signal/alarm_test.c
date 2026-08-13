#include <stdio.h>
#include <unistd.h>
int main() {
    alarm(3); // After 1s timeout, kernel sends SIGALRM to terminate the
              // process; only supports second-level timing
    long long i = 0;
    while (true) {
        i++;
        printf("processing...%lld\n", i);
    }
    return 0;
}
