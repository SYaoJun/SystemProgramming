# Extern
- 在C代码中，当你在头文件中定义了一个全局变量时，最好用extern来做声明，在c中去做实现。
```
// a.h
extern int a;
// a.c
int a = 1;
```