#include <stdio.h>

int main() {
    int a = 0x12345678;
    // 通过指针的方式判断大小端

    char* p = (char*) &a;
    if (*p == 0x78) {
        printf("little endian\n");
    } else if (*p == 0x12) {
        printf("big endian\n");
    }

    return 0;
}
