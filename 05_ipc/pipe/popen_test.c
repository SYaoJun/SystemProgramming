#include <stdio.h>
#include <stdlib.h>

int main() {
    FILE* fp;

    // 使用 popen 执行可执行文件 A，并打开写管道
    fp = popen("./a.out", "w");
    if (fp == NULL) {
        perror("popen");
        exit(EXIT_FAILURE);
    }

    // 向 A 的标准输入写入数据
    fprintf(fp, "Hello, A!\n");
    fprintf(fp, "Another line\n");

    // 关闭管道，这会导致 A 读取到 EOF 并退出
    if (pclose(fp) == -1) {
        perror("pclose");
        exit(EXIT_FAILURE);
    }

    printf("A has exited\n");

    return 0;
}
