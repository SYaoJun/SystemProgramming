/*
 * 缓冲 I/O 示例
 *
 * 演示使用标准 C 库的 fread() 和 fwrite() 进行文件 I/O
 * 缓冲 I/O 在用户空间维护缓冲区，减少系统调用次数，提高性能
 */

#include <errno.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define BUFFER_SIZE 4096

/*
 * 使用缓冲 I/O 写入文件
 *
 * fwrite() 是 C 库函数，数据先写入用户空间的缓冲区
 * 当缓冲区满或调用 fflush() 时，才调用 write() 系统调用
 *
 * 优点：减少系统调用次数，提高性能
 * 缺点：需要手动调用 fflush() 确保数据写入内核
 */
int write_file(const char* filename, const char* data) {
    FILE*  fp;
    size_t bytes_written;
    size_t data_len = strlen(data);

    printf("=== 缓冲 I/O 写入示例 ===\n");

    // 打开文件，"w" 模式表示写入，文件不存在则创建，存在则清空
    fp = fopen(filename, "w");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    // 设置缓冲区大小
    // 使用 setvbuf() 可以自定义缓冲区大小和缓冲方式
    // _IOFBF: 全缓冲，缓冲区满时才写入
    // _IOLBF: 行缓冲，遇到换行符时写入
    // _IONBF: 无缓冲，直接写入
    static char buffer[BUFFER_SIZE];
    if (setvbuf(fp, buffer, _IOFBF, BUFFER_SIZE) != 0) {
        perror("setvbuf");
        fclose(fp);
        return -1;
    }

    // 写入数据
    // fwrite() 可能不会一次写入所有数据，需要循环处理
    bytes_written = fwrite(data, 1, data_len, fp);
    if (bytes_written != data_len) {
        if (ferror(fp)) {
            perror("fwrite");
        }
        fclose(fp);
        return -1;
    }

    printf("写入 %zu 字节到文件 %s\n", bytes_written, filename);

    // fflush() 将用户空间缓冲区的数据刷新到内核的 page cache
    // 这会调用 write() 系统调用
    if (fflush(fp) != 0) {
        perror("fflush");
        fclose(fp);
        return -1;
    }

    printf("fflush 完成，数据已刷新到内核 page cache\n");

    // fileno() 获取文件描述符
    // fsync() 确保数据从 page cache 刷新到磁盘
    int fd = fileno(fp);
    if (fsync(fd) < 0) {
        perror("fsync");
        fclose(fp);
        return -1;
    }

    printf("fsync 完成，数据已刷新到磁盘\n");

    fclose(fp);
    return 0;
}

/*
 * 使用缓冲 I/O 读取文件
 *
 * fread() 是 C 库函数，从内核的 page cache 读取数据到用户空间缓冲区
 * 当缓冲区空时，才调用 read() 系统调用
 *
 * 优点：减少系统调用次数，提高性能
 */
int read_file(const char* filename) {
    FILE*  fp;
    char   buffer[BUFFER_SIZE];
    size_t bytes_read;
    size_t total_read = 0;

    printf("\n=== 缓冲 I/O 读取示例 ===\n");

    // 打开文件，"r" 模式表示读取
    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    // 设置缓冲区
    static char read_buffer[BUFFER_SIZE];
    if (setvbuf(fp, read_buffer, _IOFBF, BUFFER_SIZE) != 0) {
        perror("setvbuf");
        fclose(fp);
        return -1;
    }

    // 循环读取，直到文件结束
    while (1) {
        bytes_read = fread(buffer, 1, BUFFER_SIZE, fp);
        if (bytes_read == 0) {
            if (ferror(fp)) {
                perror("fread");
                fclose(fp);
                return -1;
            }
            if (feof(fp)) {
                // 文件结束
                break;
            }
        }

        total_read += bytes_read;
        printf("读取 %zu 字节: %.*s\n", bytes_read, (int) bytes_read, buffer);
    }

    printf("总共读取 %zu 字节\n", total_read);

    fclose(fp);
    return 0;
}

/*
 * 使用行缓冲读取文件
 *
 * 行缓冲模式在遇到换行符时自动刷新缓冲区
 * 适合逐行处理文本文件
 */
int read_line_by_line(const char* filename) {
    FILE* fp;
    char  line[256];
    int   line_num = 0;

    printf("\n=== 逐行读取示例 ===\n");

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    // 设置行缓冲
    // _IOLBF: 行缓冲，遇到换行符时刷新
    if (setvbuf(fp, NULL, _IOLBF, 0) != 0) {
        perror("setvbuf");
        fclose(fp);
        return -1;
    }

    // 使用 fgets() 逐行读取
    while (fgets(line, sizeof(line), fp) != NULL) {
        line_num++;
        printf("第 %d 行: %s", line_num, line);
    }

    if (ferror(fp)) {
        perror("fgets");
        fclose(fp);
        return -1;
    }

    printf("总共读取 %d 行\n", line_num);

    fclose(fp);
    return 0;
}

/*
 * 使用 fprintf() 格式化写入
 *
 * fprintf() 类似于 printf()，但输出到文件
 */
int fprintf_example(const char* filename) {
    FILE* fp;
    int   value = 42;
    float pi    = 3.14159f;

    printf("\n=== fprintf 格式化写入示例 ===\n");

    fp = fopen(filename, "a"); // "a" 模式表示追加
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    // 格式化写入
    fprintf(fp, "Integer: %d\n", value);
    fprintf(fp, "Float: %.2f\n", pi);
    fprintf(fp, "String: %s\n", "Hello, fprintf!");

    printf("格式化数据已追加到文件\n");

    fclose(fp);
    return 0;
}

/*
 * 使用 fscanf() 格式化读取
 *
 * fscanf() 类似于 scanf()，但从文件读取
 */
int fscanf_example(const char* filename) {
    FILE* fp;
    int   value;
    float pi;
    char  str[64];

    printf("\n=== fscanf 格式化读取示例 ===\n");

    fp = fopen(filename, "r");
    if (fp == NULL) {
        perror("fopen");
        return -1;
    }

    // 格式化读取
    // 注意：fscanf() 的格式字符串要与写入时匹配
    if (fscanf(fp, "Integer: %d\n", &value) == 1) {
        printf("读取整数: %d\n", value);
    }
    if (fscanf(fp, "Float: %f\n", &pi) == 1) {
        printf("读取浮点数: %.2f\n", pi);
    }
    if (fscanf(fp, "String: %63s\n", str) == 1) {
        printf("读取字符串: %s\n", str);
    }

    fclose(fp);
    return 0;
}

int main() {
    const char* filename = "buffered_io_test.txt";
    const char* data = "Hello, Buffered I/O!\nThis is a test file for buffered "
                       "I/O operations.\n";

    // 写入文件
    if (write_file(filename, data) < 0) {
        fprintf(stderr, "写入文件失败\n");
        return 1;
    }

    // 读取文件
    if (read_file(filename) < 0) {
        fprintf(stderr, "读取文件失败\n");
        return 1;
    }

    // 逐行读取
    if (read_line_by_line(filename) < 0) {
        fprintf(stderr, "逐行读取失败\n");
        return 1;
    }

    // 格式化写入
    if (fprintf_example(filename) < 0) {
        fprintf(stderr, "格式化写入失败\n");
        return 1;
    }

    // 格式化读取
    if (fscanf_example(filename) < 0) {
        fprintf(stderr, "格式化读取失败\n");
        return 1;
    }

    printf("\n所有示例完成\n");
    return 0;
}
