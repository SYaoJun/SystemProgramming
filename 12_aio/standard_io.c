/*
 * 标准 I/O 示例
 *
 * 演示使用 read() 和 write() 系统调用进行文件 I/O
 * 这是最基础的文件 I/O 方式，直接使用系统调用
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

/*
 * 使用标准 I/O 写入文件
 *
 * write() 是系统调用，直接将数据从用户空间复制到内核的 page cache
 */
int write_file(const char* filename, const char* data) {
    int     fd;
    ssize_t bytes_written;
    size_t  total_written = 0;
    size_t  data_len      = strlen(data);

    printf("=== 标准 I/O 写入示例 ===\n");

    // 打开文件，O_CREAT 表示不存在则创建，O_TRUNC 表示清空文件
    // O_WRONLY 表示只写模式
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    // 循环写入，直到所有数据都写入完成
    // write() 可能不会一次写入所有数据，需要循环处理
    while (total_written < data_len) {
        bytes_written
            = write(fd, data + total_written, data_len - total_written);
        if (bytes_written < 0) {
            if (errno == EINTR) {
                // 被信号中断，继续写入
                continue;
            }
            perror("write");
            close(fd);
            return -1;
        }
        total_written += bytes_written;
    }

    printf("写入 %zu 字节到文件 %s\n", total_written, filename);

    // fsync() 确保数据从 page cache 刷新到磁盘
    // 这是一个阻塞调用，会等待数据真正写入磁盘
    if (fsync(fd) < 0) {
        perror("fsync");
        close(fd);
        return -1;
    }

    printf("fsync 完成，数据已刷新到磁盘\n");

    close(fd);
    return 0;
}

/*
 * 使用标准 I/O 读取文件
 *
 * read() 是系统调用，从内核的 page cache 读取数据到用户空间
 * 如果数据不在 page cache 中，会先从磁盘读取到 page cache
 */
int read_file(const char* filename) {
    int     fd;
    char    buffer[BUFFER_SIZE];
    ssize_t bytes_read;
    size_t  total_read = 0;

    printf("\n=== 标准 I/O 读取示例 ===\n");

    // 打开文件，O_RDONLY 表示只读模式
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    // 循环读取，直到文件结束
    // read() 可能不会一次读取所有数据，需要循环处理
    while (1) {
        bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            if (errno == EINTR) {
                // 被信号中断，继续读取
                continue;
            }
            perror("read");
            close(fd);
            return -1;
        }
        if (bytes_read == 0) {
            // 文件结束
            break;
        }

        total_read += bytes_read;
        printf("读取 %zd 字节: %.*s\n", bytes_read, (int) bytes_read, buffer);
    }

    printf("总共读取 %zu 字节\n", total_read);

    close(fd);
    return 0;
}

/*
 * 使用 lseek() 定位文件偏移
 *
 * lseek() 用于移动文件读写位置
 * 可以用于随机访问文件
 */
int lseek_example(const char* filename) {
    int     fd;
    char    buffer[128];
    ssize_t bytes_read;

    printf("\n=== lseek 定位示例 ===\n");

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    // 从文件开头读取前 10 个字节
    lseek(fd, 0, SEEK_SET);
    bytes_read = read(fd, buffer, 10);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("文件开头 10 字节: %s\n", buffer);
    }

    // 从文件末尾往前读取 10 个字节
    lseek(fd, -10, SEEK_END);
    bytes_read = read(fd, buffer, 10);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("文件末尾 10 字节: %s\n", buffer);
    }

    // 从当前位置往后读取 10 个字节
    lseek(fd, 10, SEEK_CUR);
    bytes_read = read(fd, buffer, 10);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("当前位置 +10 字节: %s\n", buffer);
    }

    close(fd);
    return 0;
}

int main() {
    const char* filename = "standard_io_test.txt";
    const char* data = "Hello, Standard I/O!\nThis is a test file for standard "
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

    // lseek 示例
    if (lseek_example(filename) < 0) {
        fprintf(stderr, "lseek 示例失败\n");
        return 1;
    }

    printf("\n所有示例完成\n");
    return 0;
}
