#include <stdio.h>
#include <unistd.h>
int main() {

    printf("main pid = %d, ppid = %d\n", getpid(), getppid());
    setvbuf(stdout, NULL, _IONBF, 0);
    printf("A\n");
    // Parent process > 0, child process = 0
    if (fork() && fork()) { //  Created two child processes
        printf("D\n");
    }
    // 1. How many child processes were created? 2
    // 2. How many times can A be printed at most? 3
    // 3. How many times can D be printed at most? 1
    sleep(100);
    return 0;
}
