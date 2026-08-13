#include <stdio.h>

// 声明外部全局变量
extern int globalVar;

void accessGlobalVar() {
    globalVar++;
    printf("Global Variable in file2: %d\n", globalVar);
}
