/*
 * Linux Native AIO (libaio) 示例
 *
 * 演示使用 Linux 原生 AIO 接口进行异步 I/O
 *
 * 注意：Linux Native AIO 主要用于 O_DIRECT 模式，对普通文件支持有限
 * 对于现代应用，推荐使用 io_uring 替代 libaio
 */

#define _GNU_SOURCE
#include <errno.h>
#include <fcntl.h>
#include <libaio.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define AIO_EVENTS 4

/*
 * 使用 libaio 异步写入文件
 *
 * libaio 允许应用程序提交多个 I/O 请求，然后等待它们完成
 * 适用于高 I/O 并发的场景
 */
int aio_write(const char* filename, const char* data) {
    int             fd, ret;
    io_context_t    ctx = 0;
    struct iocb     cb;
    struct io_event events[AIO_EVENTS];
    char*           aligned_buffer;
    size_t          data_len = strlen(data);

    printf("=== libaio 异步写入示例 ===\n");

    // 分配对齐的缓冲区（O_DIRECT 要求）
    // posix_memalign 分配的内存地址是指定大小的倍数
    if (posix_memalign((void**) &aligned_buffer, 512, BUFFER_SIZE) != 0) {
        perror("posix_memalign");
        return -1;
    }
    memset(aligned_buffer, 0, BUFFER_SIZE);
    strncpy(aligned_buffer, data, BUFFER_SIZE - 1);

    // 初始化 io_context
    // io_context_t 是 AIO 上下文，用于管理 AIO 请求
    ret = io_setup(AIO_EVENTS, &ctx);
    if (ret < 0) {
        perror("io_setup");
        free(aligned_buffer);
        return -1;
    }

    // 打开文件，使用 O_DIRECT 绕过 page cache
    // libaio 主要用于 O_DIRECT 模式
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_DIRECT, 0644);
    if (fd < 0) {
        perror("open with O_DIRECT");
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    // 准备 iocb 结构
    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes     = fd;
    cb.aio_lio_opcode = IO_CMD_PWRITE; // 异步写入
    cb.aio_buf        = (unsigned long) aligned_buffer;
    cb.aio_nbytes     = data_len;
    cb.aio_offset     = 0;

    // 提交 AIO 请求
    // io_submit 将 iocb 提交到内核
    ret = io_submit(ctx, 1, &cb);
    if (ret < 0) {
        perror("io_submit");
        close(fd);
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    printf("AIO 写入请求已提交\n");

    // 等待 AIO 请求完成
    // io_getevents 等待指定数量的 I/O 事件完成
    ret = io_getevents(ctx, 1, 1, events, NULL);
    if (ret < 0) {
        perror("io_getevents");
        close(fd);
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    // 检查 I/O 结果
    if (events[0].res < 0) {
        fprintf(stderr, "AIO 写入失败: %s\n", strerror(-events[0].res));
        close(fd);
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    printf("AIO 写入完成，写入 %ld 字节\n", events[0].res);

    // fsync 确保数据刷新到磁盘
    if (fsync(fd) < 0) {
        perror("fsync");
        close(fd);
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    printf("fsync 完成\n");

    // 清理资源
    close(fd);
    io_destroy(ctx);
    free(aligned_buffer);

    return 0;
}

/*
 * 使用 libaio 异步读取文件
 */
int aio_read(const char* filename) {
    int             fd, ret;
    io_context_t    ctx = 0;
    struct iocb     cb;
    struct io_event events[AIO_EVENTS];
    char*           aligned_buffer;

    printf("\n=== libaio 异步读取示例 ===\n");

    // 分配对齐的缓冲区
    if (posix_memalign((void**) &aligned_buffer, 512, BUFFER_SIZE) != 0) {
        perror("posix_memalign");
        return -1;
    }
    memset(aligned_buffer, 0, BUFFER_SIZE);

    // 初始化 io_context
    ret = io_setup(AIO_EVENTS, &ctx);
    if (ret < 0) {
        perror("io_setup");
        free(aligned_buffer);
        return -1;
    }

    // 打开文件，使用 O_DIRECT
    fd = open(filename, O_RDONLY | O_DIRECT);
    if (fd < 0) {
        perror("open with O_DIRECT");
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    // 准备 iocb 结构
    memset(&cb, 0, sizeof(cb));
    cb.aio_fildes     = fd;
    cb.aio_lio_opcode = IO_CMD_PREAD; // 异步读取
    cb.aio_buf        = (unsigned long) aligned_buffer;
    cb.aio_nbytes     = BUFFER_SIZE;
    cb.aio_offset     = 0;

    // 提交 AIO 请求
    ret = io_submit(ctx, 1, &cb);
    if (ret < 0) {
        perror("io_submit");
        close(fd);
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    printf("AIO 读取请求已提交\n");

    // 等待 AIO 请求完成
    ret = io_getevents(ctx, 1, 1, events, NULL);
    if (ret < 0) {
        perror("io_getevents");
        close(fd);
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    // 检查 I/O 结果
    if (events[0].res < 0) {
        fprintf(stderr, "AIO 读取失败: %s\n", strerror(-events[0].res));
        close(fd);
        io_destroy(ctx);
        free(aligned_buffer);
        return -1;
    }

    printf("AIO 读取完成，读取 %ld 字节: %s\n", events[0].res, aligned_buffer);

    // 清理资源
    close(fd);
    io_destroy(ctx);
    free(aligned_buffer);

    return 0;
}

/*
 * 使用 libaio 批量 I/O
 *
 * 演示同时提交多个 I/O 请求
 */
int aio_batch_io(const char* filename) {
    int             fd, ret, i;
    io_context_t    ctx = 0;
    struct iocb     cbs[AIO_EVENTS];
    struct iocb*    cb_ptrs[AIO_EVENTS];
    struct io_event events[AIO_EVENTS];
    char*           aligned_buffers[AIO_EVENTS];
    const char*     data = "Batch AIO test data\n";

    printf("\n=== libaio 批量 I/O 示例 ===\n");

    // 初始化 io_context
    ret = io_setup(AIO_EVENTS, &ctx);
    if (ret < 0) {
        perror("io_setup");
        return -1;
    }

    // 打开文件
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_DIRECT, 0644);
    if (fd < 0) {
        perror("open with O_DIRECT");
        io_destroy(ctx);
        return -1;
    }

    // 准备多个 AIO 请求
    for (i = 0; i < AIO_EVENTS; i++) {
        // 分配对齐的缓冲区
        if (posix_memalign((void**) &aligned_buffers[i], 512, BUFFER_SIZE)
            != 0) {
            perror("posix_memalign");
            // 清理已分配的缓冲区
            for (int j = 0; j < i; j++) {
                free(aligned_buffers[j]);
            }
            close(fd);
            io_destroy(ctx);
            return -1;
        }
        memset(aligned_buffers[i], 0, BUFFER_SIZE);
        strncpy(aligned_buffers[i], data, BUFFER_SIZE - 1);

        // 准备 iocb
        memset(&cbs[i], 0, sizeof(struct iocb));
        cbs[i].aio_fildes     = fd;
        cbs[i].aio_lio_opcode = IO_CMD_PWRITE;
        cbs[i].aio_buf        = (unsigned long) aligned_buffers[i];
        cbs[i].aio_nbytes     = strlen(data);
        cbs[i].aio_offset     = i * BUFFER_SIZE; // 不同的偏移

        cb_ptrs[i] = &cbs[i];
    }

    // 批量提交 AIO 请求
    ret = io_submit(ctx, AIO_EVENTS, cb_ptrs);
    if (ret < 0) {
        perror("io_submit");
        for (i = 0; i < AIO_EVENTS; i++) {
            free(aligned_buffers[i]);
        }
        close(fd);
        io_destroy(ctx);
        return -1;
    }

    printf("批量提交 %d 个 AIO 请求\n", AIO_EVENTS);

    // 等待所有 AIO 请求完成
    ret = io_getevents(ctx, AIO_EVENTS, AIO_EVENTS, events, NULL);
    if (ret < 0) {
        perror("io_getevents");
        for (i = 0; i < AIO_EVENTS; i++) {
            free(aligned_buffers[i]);
        }
        close(fd);
        io_destroy(ctx);
        return -1;
    }

    printf("所有 AIO 请求完成\n");

    // 清理资源
    for (i = 0; i < AIO_EVENTS; i++) {
        free(aligned_buffers[i]);
    }
    close(fd);
    io_destroy(ctx);

    return 0;
}

int main() {
    const char* filename = "libaio_test.txt";
    const char* data
        = "Hello, libaio!\nThis is a test file for Linux native AIO.\n";

    // 异步写入
    if (aio_write(filename, data) < 0) {
        fprintf(stderr, "AIO 写入失败\n");
        return 1;
    }

    // 异步读取
    if (aio_read(filename) < 0) {
        fprintf(stderr, "AIO 读取失败\n");
        return 1;
    }

    // 批量 I/O
    if (aio_batch_io(filename) < 0) {
        fprintf(stderr, "批量 AIO 失败\n");
        return 1;
    }

    printf("\n所有 libaio 示例完成\n");
    printf("注意：libaio 主要用于 O_DIRECT 模式，对普通文件支持有限\n");
    printf("对于现代应用，推荐使用 io_uring 替代 libaio\n");

    return 0;
}
