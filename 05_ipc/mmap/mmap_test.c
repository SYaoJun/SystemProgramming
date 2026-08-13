#include <stdio.h>
#include <stdlib.h>
#include <sys/mman.h>

#define GiB *(1024LL * 1024 * 1024)
int main() {
    void* p = mmap(NULL, 3 GiB, PROT_READ | PROT_WRITE,
        MAP_ANONYMOUS | MAP_PRIVATE, -1, 0);
    if (p == MAP_FAILED) {
        printf("mmap failed!\n");
    } else {
        printf("mmap success!\n");
    }

    *(int*) ((u_int8_t*) p + 1 GiB) = 114;
    *(int*) ((u_int8_t*) p + 2 GiB) = 115;

    printf("read = %d\n", *(int*) ((u_int8_t*) p + 2 GiB));
    return 0;
}
