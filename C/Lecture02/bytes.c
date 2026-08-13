#include <stdio.h>
#include <unistd.h>
#include<stdint.h>
int main(){
    int8_t t = -1;
    printf("original int8 t = %d\n", t);
    printf("unsigned uint8 t = %u\n", (uint8_t)(t));
    printf("unsigned uint8 t = %x\n", (uint8_t)(t));
    return 0;
}