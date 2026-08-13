
#include <stdio.h>
int (*my_func_ptr)(int a, int b);
int add(int a, int b) {
    return a + b;
}
int main() {
    my_func_ptr = add;
    printf("res = %d\n", my_func_ptr(1, 2));
    return 0;
}
