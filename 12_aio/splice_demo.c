/*
 * splice 管道 I/O 示例
 *
 * 演示使用 splice() 系统调用在文件描述符之间移动数据
 *
 * splice() 是 Linux 特有的系统调用，可以在两个文件描述符之间移动数据
 * 无需经过用户空间，实现零拷贝
 *
 * 常用于：
 * - 文件到管道
 * - 管道到文件
 * - 管道到管道
 * - 文件到 socket（通过管道）
 */

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 4096

/*
 * 使用 splice() 将文件数据移动到管道
 *
 * splice() 可以在文件和管道之间移动数据
 * 数据直接在内核空间传输，无需经过用户空间
 */
int splice_file_to_pipe(const char* filename) {
    int     fd, pipe_fd[2];
    ssize_t bytes_spliced;
    char    buffer[BUFFER_SIZE];

    printf("=== splice 文件到管道示例 ===\n");

    // 创建管道
    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        return -1;
    }

    // 打开文件
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    // 使用 splice 将文件数据移动到管道
    // splice() 参数：
    // - fd_in: 输入文件描述符
    // - off_in: 输入偏移量（NULL 表示从当前位置开始）
    // - fd_out: 输出文件描述符
    // - off_out: 输出偏移量（NULL 表示从当前位置开始）
    // - len: 要移动的字节数
    // - flags: 标志位
    bytes_spliced = splice(fd, NULL, pipe_fd[1], NULL, BUFFER_SIZE, 0);
    if (bytes_spliced < 0) {
        perror("splice");
        close(fd);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    printf("splice 移动了 %zd 字节到管道\n", bytes_spliced);

    // 从管道读取数据（验证）
    close(pipe_fd[1]); // 关闭写端
    ssize_t bytes_read = read(pipe_fd[0], buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("从管道读取 %zd 字节: %s\n", bytes_read, buffer);
    }

    close(fd);
    close(pipe_fd[0]);

    return 0;
}

/*
 * 使用 splice() 将管道数据移动到文件
 *
 * 演示从管道读取数据并写入文件
 */
int splice_pipe_to_file(const char* filename) {
    int         fd, pipe_fd[2];
    ssize_t     bytes_spliced;
    const char* data = "Hello, splice!\nThis data goes through a pipe.\n";

    printf("\n=== splice 管道到文件示例 ===\n");

    // 创建管道
    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        return -1;
    }

    // 向管道写入数据
    write(pipe_fd[1], data, strlen(data));
    close(pipe_fd[1]); // 关闭写端

    // 打开目标文件
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        close(pipe_fd[0]);
        return -1;
    }

    // 使用 splice 将管道数据移动到文件
    bytes_spliced = splice(pipe_fd[0], NULL, fd, NULL, BUFFER_SIZE, 0);
    if (bytes_spliced < 0) {
        perror("splice");
        close(fd);
        close(pipe_fd[0]);
        return -1;
    }

    printf("splice 移动了 %zd 字节到文件\n", bytes_spliced);

    if (fsync(fd) < 0) {
        perror("fsync");
        close(fd);
        close(pipe_fd[0]);
        return -1;
    }

    close(fd);
    close(pipe_fd[0]);

    return 0;
}

/*
 * 使用 splice() 在两个管道之间移动数据
 *
 * 演示管道到管道的数据移动
 */
int splice_pipe_to_pipe() {
    int         pipe1[2], pipe2[2];
    ssize_t     bytes_spliced;
    const char* data = "Data from pipe1 to pipe2\n";
    char        buffer[BUFFER_SIZE];

    printf("\n=== splice 管道到管道示例 ===\n");

    // 创建两个管道
    if (pipe(pipe1) < 0) {
        perror("pipe1");
        return -1;
    }
    if (pipe(pipe2) < 0) {
        perror("pipe2");
        close(pipe1[0]);
        close(pipe1[1]);
        return -1;
    }

    // 向第一个管道写入数据
    write(pipe1[1], data, strlen(data));
    close(pipe1[1]); // 关闭写端

    // 使用 splice 将数据从 pipe1 移动到 pipe2
    bytes_spliced = splice(pipe1[0], NULL, pipe2[1], NULL, BUFFER_SIZE, 0);
    if (bytes_spliced < 0) {
        perror("splice");
        close(pipe1[0]);
        close(pipe2[0]);
        close(pipe2[1]);
        return -1;
    }

    printf("splice 移动了 %zd 字节从 pipe1 到 pipe2\n", bytes_spliced);

    close(pipe2[1]); // 关闭写端

    // 从 pipe2 读取数据（验证）
    ssize_t bytes_read = read(pipe2[0], buffer, BUFFER_SIZE);
    if (bytes_read > 0) {
        buffer[bytes_read] = '\0';
        printf("从 pipe2 读取 %zd 字节: %s\n", bytes_read, buffer);
    }

    close(pipe1[0]);
    close(pipe2[0]);

    return 0;
}

/*
 * 使用 splice() 分块传输大文件
 *
 * 对于大文件，可以分块使用 splice 传输
 */
int splice_chunked(const char* src_file, const char* dst_file) {
    int         src_fd, dst_fd, pipe_fd[2];
    struct stat stat_buf;
    ssize_t     bytes_spliced;
    size_t      chunk_size    = 1024 * 1024; // 1MB chunks
    size_t      total_spliced = 0;

    printf("\n=== splice 分块传输示例 ===\n");

    // 创建管道
    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        return -1;
    }

    // 打开源文件
    src_fd = open(src_file, O_RDONLY);
    if (src_fd < 0) {
        perror("open src file");
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    // 获取文件大小
    if (fstat(src_fd, &stat_buf) < 0) {
        perror("fstat");
        close(src_fd);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    printf("源文件大小: %ld 字节\n", stat_buf.st_size);

    // 打开目标文件
    dst_fd = open(dst_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        perror("open dst file");
        close(src_fd);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    // 分块传输：文件 -> 管道 -> 文件
    while (total_spliced < stat_buf.st_size) {
        size_t to_splice = chunk_size;
        if (stat_buf.st_size - total_spliced < chunk_size) {
            to_splice = stat_buf.st_size - total_spliced;
        }

        // 文件到管道
        bytes_spliced = splice(src_fd, NULL, pipe_fd[1], NULL, to_splice, 0);
        if (bytes_spliced < 0) {
            perror("splice (file to pipe)");
            close(src_fd);
            close(dst_fd);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            return -1;
        }

        // 管道到文件
        bytes_spliced
            = splice(pipe_fd[0], NULL, dst_fd, NULL, bytes_spliced, 0);
        if (bytes_spliced < 0) {
            perror("splice (pipe to file)");
            close(src_fd);
            close(dst_fd);
            close(pipe_fd[0]);
            close(pipe_fd[1]);
            return -1;
        }

        total_spliced += bytes_spliced;
        printf("已传输 %zu / %ld 字节 (%.1f%%)\n", total_spliced,
            stat_buf.st_size, (double) total_spliced / stat_buf.st_size * 100);
    }

    printf("分块传输完成，总共 %zu 字节\n", total_spliced);

    if (fsync(dst_fd) < 0) {
        perror("fsync");
        close(src_fd);
        close(dst_fd);
        close(pipe_fd[0]);
        close(pipe_fd[1]);
        return -1;
    }

    close(src_fd);
    close(dst_fd);
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    return 0;
}

/*
 * 使用 splice() 配合 tee() 复制数据
 *
 * tee() 可以在管道之间复制数据，而不消耗数据
 * 结合 splice() 可以实现数据复制和移动
 */
int splice_with_tee() {
    int         pipe1[2], pipe2[2];
    ssize_t     bytes_teed, bytes_spliced;
    const char* data = "Data for tee and splice\n";
    char        buffer1[BUFFER_SIZE], buffer2[BUFFER_SIZE];

    printf("\n=== splice 配合 tee 示例 ===\n");

    // 创建两个管道
    if (pipe(pipe1) < 0) {
        perror("pipe1");
        return -1;
    }
    if (pipe(pipe2) < 0) {
        perror("pipe2");
        close(pipe1[0]);
        close(pipe1[1]);
        return -1;
    }

    // 向第一个管道写入数据
    write(pipe1[1], data, strlen(data));
    close(pipe1[1]);

    // 使用 tee() 将数据从 pipe1 复制到 pipe2
    // tee() 不会消耗 pipe1 中的数据
    bytes_teed = tee(pipe1[0], pipe2[1], BUFFER_SIZE, 0);
    if (bytes_teed < 0) {
        perror("tee");
        close(pipe1[0]);
        close(pipe2[0]);
        close(pipe2[1]);
        return -1;
    }

    printf("tee 复制了 %zd 字节\n", bytes_teed);

    close(pipe2[1]);

    // 从 pipe2 读取（验证复制）
    ssize_t bytes_read = read(pipe2[0], buffer2, BUFFER_SIZE);
    if (bytes_read > 0) {
        buffer2[bytes_read] = '\0';
        printf("从 pipe2 读取 %zd 字节: %s\n", bytes_read, buffer2);
    }

    // 从 pipe1 读取（验证数据还在）
    bytes_read = read(pipe1[0], buffer1, BUFFER_SIZE);
    if (bytes_read > 0) {
        buffer1[bytes_read] = '\0';
        printf("从 pipe1 读取 %zd 字节: %s\n", bytes_read, buffer1);
    }

    close(pipe1[0]);
    close(pipe2[0]);

    return 0;
}

/*
 * 对比 splice 和传统 read/write
 *
 * splice: 零拷贝，直接在内核空间传输
 * read/write: 数据在用户空间和内核空间之间来回拷贝
 */
int compare_splice_traditional(const char* src_file,
    const char* dst_file_splice, const char* dst_file_traditional) {
    int         src_fd, dst_fd, pipe_fd[2];
    struct stat stat_buf;
    char        buffer[BUFFER_SIZE];
    ssize_t     bytes_read, bytes_written, bytes_spliced;

    printf("\n=== splice vs 传统方式对比 ===\n");

    src_fd = open(src_file, O_RDONLY);
    if (src_fd < 0) {
        perror("open src file");
        return -1;
    }

    if (fstat(src_fd, &stat_buf) < 0) {
        perror("fstat");
        close(src_fd);
        return -1;
    }

    printf("文件大小: %ld 字节\n", stat_buf.st_size);

    // 传统方式：read + write
    dst_fd = open(dst_file_traditional, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        perror("open dst file (traditional)");
        close(src_fd);
        return -1;
    }

    printf("使用传统 read/write 方式...\n");
    while ((bytes_read = read(src_fd, buffer, BUFFER_SIZE)) > 0) {
        bytes_written = write(dst_fd, buffer, bytes_read);
        if (bytes_written < 0) {
            perror("write");
            close(src_fd);
            close(dst_fd);
            return -1;
        }
    }

    fsync(dst_fd);
    close(dst_fd);
    close(src_fd);

    // splice 方式
    if (pipe(pipe_fd) < 0) {
        perror("pipe");
        return -1;
    }

    src_fd = open(src_file, O_RDONLY);
    dst_fd = open(dst_file_splice, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    printf("使用 splice 方式...\n");
    bytes_spliced = splice(src_fd, NULL, pipe_fd[1], NULL, stat_buf.st_size, 0);
    if (bytes_spliced > 0) {
        splice(pipe_fd[0], NULL, dst_fd, NULL, bytes_spliced, 0);
    }

    fsync(dst_fd);
    close(dst_fd);
    close(src_fd);
    close(pipe_fd[0]);
    close(pipe_fd[1]);

    printf("对比完成\n");
    printf("传统方式：数据在用户空间和内核空间之间来回拷贝\n");
    printf("splice：零拷贝，直接在内核空间传输\n");

    return 0;
}

int main() {
    const char* src_file        = "splice_src.txt";
    const char* dst_file        = "splice_dst.txt";
    const char* dst_chunked     = "splice_chunked.txt";
    const char* dst_splice      = "splice_splice.txt";
    const char* dst_traditional = "splice_traditional.txt";

    // 创建测试文件
    int fd = open(src_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        const char* data
            = "Hello, splice!\nThis is a test file for splice I/O.\n"
              "splice is efficient for data movement between file "
              "descriptors.\n";
        for (int i = 0; i < 1000; i++) {
            write(fd, data, strlen(data));
        }
        fsync(fd);
        close(fd);
    }

    // splice 文件到管道
    if (splice_file_to_pipe(src_file) < 0) {
        fprintf(stderr, "splice 文件到管道失败\n");
    }

    // splice 管道到文件
    if (splice_pipe_to_file(dst_file) < 0) {
        fprintf(stderr, "splice 管道到文件失败\n");
    }

    // splice 管道到管道
    if (splice_pipe_to_pipe() < 0) {
        fprintf(stderr, "splice 管道到管道失败\n");
    }

    // splice 分块传输
    if (splice_chunked(src_file, dst_chunked) < 0) {
        fprintf(stderr, "splice 分块传输失败\n");
    }

    // splice 配合 tee
    if (splice_with_tee() < 0) {
        fprintf(stderr, "splice 配合 tee 失败\n");
    }

    // 性能对比
    if (compare_splice_traditional(src_file, dst_splice, dst_traditional) < 0) {
        fprintf(stderr, "性能对比失败\n");
    }

    printf("\n所有 splice 示例完成\n");
    return 0;
}
