#include <stdio.h>

// 定义回调函数类型
typedef int (*CallbackFunction)(int, int);

// 定义回调函数
int add(int a, int b) {
    return a + b;
}

int multiply(int a, int b) {
    return a * b;
}

// 使用回调函数的函数
int compute(int a, int b, CallbackFunction callback) {
    return callback(a, b);
}

int main() {
    int x = 5, y = 10;

    // 使用回调函数进行加法运算
    int sum = compute(x, y, add);
    printf("Sum: %d\n", sum);

    // 使用回调函数进行乘法运算
    int product = compute(x, y, multiply);
    printf("Product: %d\n", product);

    return 0;
}
