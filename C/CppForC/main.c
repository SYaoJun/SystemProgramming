// main.c
#include <stdio.h>
#include "MyClassC.h"

int main() {
    MyClassWrapper* myClass = MyClass_create();
    int result = MyClass_add(myClass, 3, 4);
    printf("3 + 4 = %d\n", result);
    MyClass_destroy(myClass);
    return 0;
}
