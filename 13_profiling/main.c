#include <stdio.h>

float float_math(float x, float y);
float float_power(float x, float y);
float float_math_helper(float x, float y);

int int_math(int x, int y);
int int_power(int x, int y);
int int_math_helper(int x, int y);

int main(void) {
    int   x1 = 10, y1 = 3, r1 = 0;
    float x2 = 10.0, y2 = 3.0, r2 = 0.0;

    for (int i = 0; i < 10000000; i++) {
        r1 += int_math(x1, y1);
        r2 += float_math(x2, y2);
    }

    return 0;
}

int int_math(int x, int y) {
    return int_math_helper(x, y);
}

float float_math(float x, float y) {
    return float_math_helper(x, y);
}

int int_math_helper(int x, int y) {
    return x / y * int_power(y, x) / int_power(x, y);
}

float float_math_helper(float x, float y) {
    return x / y * float_power(y, x) / float_power(x, y);
}

int int_power(int x, int y) {
    int r = x;
    for (int i = 1; i < y; i++) {
        r = r * x;
    }
    return r;
}

float float_power(float x, float y) {
    float r = x;
    for (int i = 1; i < y; i++) {
        r = r * x;
    }
    return r;
}
