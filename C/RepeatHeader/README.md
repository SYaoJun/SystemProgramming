# 头文件定义
如果多个文件重复引用了一个头文件，就可能有问题。
```c
#ifndef FILE_3_H
#define FILE_3_H

#define N 100
int f(int a);
extern int score;

#endif
```
