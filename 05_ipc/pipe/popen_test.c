#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fp;

    // Use popen to execute executable A and open a write pipe
    fp = popen("./a.out", "w");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    // Write data to A's stdin
    fprintf(fp, "Hello, A!\n");
    fprintf(fp, "Another line\n");

    // Close the pipe, which causes A to read EOF and exit
    if (pclose(fp) == -1) {
        perror("pclose");
        exit(EXIT_FAILURE);
    }

    printf("A has exited\n");

    return 0;
}
