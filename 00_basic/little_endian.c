#include <stdio.h>

int main() {
    int a = 0x12345678;
    // Determine endianness via pointer

    char* p = (char*) &a;
    if (*p == 0x78) {
        printf("little endian\n");
    } else if (*p == 0x12) {
        printf("big endian\n");
    }

    return 0;
}
