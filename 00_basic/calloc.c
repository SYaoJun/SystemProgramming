#include <stdio.h>
#include <stdlib.h>

int main() {
    int* malloc_ptr = (int*) malloc(20 * sizeof(int));
    int* calloc_ptr = (int*) calloc(20, sizeof(int));

    for (int i = 0; i < 20; i++) {
        printf("%d ", *malloc_ptr++);
    }
    puts("");

    for (int i = 0; i < 20; i++) {
        printf("%d ", *calloc_ptr++);
    }
    puts("");
    return 0;
}
