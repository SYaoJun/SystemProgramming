```
# 会生成一个gmon.out文件
gcc main.c -o main -pg
gprof main -b -a
```