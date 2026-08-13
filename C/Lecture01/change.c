
#include <stdio.h>
#include <math.h>
int main(){ 
    float price = 0;
    printf("Please enter the price (yuan): ");
    // 从命令行读入数据
    scanf("%f", &price);
    float change = 100 - price;
    // 向下取整floor
    // 向上取整 ceil
    float pi = 3.1415926;
    // 2*pi*r 
    printf("floor: Change: %.3f yuan.\n",  floor(change));
    printf("ceil: Change: %.3f yuan.\n",  ceil(change));
    printf("round: pi = %.3f yuan.\n",  round(pi));
    // int 占4字节 字节 Byte = 8bit 位 01 = 32位 
    int a = 5;
    // 用1bit表示正负
    // 负数是用补码表示的

    // 5 = 0  000 0000 0000 0000 0000 0000 0000  0101
    // 0000 
    // 1111 = 15 = 2^4 -1
    // 111 = 7 = 2^3 -1
    // 11 = 3 = 2^2 -1
    //  1 = 1 = 2^1 -1
    long long b = 1ll << 63;
    scanf("%f", price);
    return 0;
}
