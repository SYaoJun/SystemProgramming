#include <stdio.h>

// 全局变量，不使用static，其他文件也能访问这个变量，只要在用前，声明extern int globalVar;
int globalVar = 0;

void incrementGlobalVar() {
    globalVar++;
}

void printGlobalVar() {
    printf("Global Variable in file1: %d\n", globalVar);
}
