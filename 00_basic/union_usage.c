#include <stdio.h>
union TEST {
    short a;
    char  b[sizeof(short)];
};
int main() {
    union TEST test;
    test.a = 0x0102; // Cannot reference the union variable itself, only its
                     // members.
    if (test.b[0] == 0x01 && test.b[1] == 0x02) {
        printf("big endian.\n");
    } else if (test.b[0] == 0x02 && test.b[1] == 0x01) {
        printf("small endian.\n");
    } else {
        printf("unknown endian.\n");
    }
    return 0;
}
