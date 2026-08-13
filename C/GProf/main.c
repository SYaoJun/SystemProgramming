// filename : main.cpp
#include <stdio.h>
#include <string.h>

#define INT32_MAX 0x3f3f3f3f

void xor_calc()
{
    int t = 0;
    for (int i = 0; i < INT32_MAX; ++i)
        t ^= i;
}

void time_cost1()
{
    for (int i = 0; i < 3; ++i)
        xor_calc();
}

void time_cost2()
{
    for (int i = 0; i < 5; ++i)
        xor_calc();
}

int main()
{
    time_cost1();
    time_cost2();
    return 0;
}
