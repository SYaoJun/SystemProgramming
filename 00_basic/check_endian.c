#include <stdio.h>

int main() {
    unsigned int   x   = 0x12345678;
    unsigned char* ptr = (unsigned char*) &x;

    if (*ptr == 0x78) {
        printf("系统是小端（Little Endian）\n");
    } else if (*ptr == 0x12) {
        printf("系统是大端（Big Endian）\n");
    } else {
        printf("无法识别字节序\n");
    }

    return 0;
}
