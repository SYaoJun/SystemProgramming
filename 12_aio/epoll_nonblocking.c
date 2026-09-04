/*
 * epoll + 非阻塞 I/O 示例
 *
 * 演示使用 epoll 监控多个文件描述符的 I/O 事件
 * 非阻塞 I/O 允许程序在没有数据可读时立即返回，而不是阻塞
 *
 * epoll 是 Linux 特有的高效 I/O 事件通知机制
 * 适用于需要同时处理大量连接的场景（如网络服务器）
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/epoll.h>
#include <sys/stat.h>
#include <unistd.h>

#define MAX_EVENTS 10
#define BUFFER_SIZE 4096

/*
 * 设置文件描述符为非阻塞模式
 *
 * 非阻塞模式下，read() 和 write() 不会阻塞
 * 如果没有数据可读或无法写入，立即返回 -1，errno 设置为 EAGAIN
 */
int set_nonblocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags < 0) {
        perror("fcntl F_GETFL");
        return -1;
    }

    if (fcntl(fd, F_SETFL, flags | O_NONBLOCK) < 0) {
        perror("fcntl F_SETFL");
        return -1;
    }

    return 0;
}

/*
 * 使用 epoll 监控文件描述符的可读事件
 *
 * epoll_wait() 会阻塞，直到有文件描述符就绪或超时
 * 返回就绪的文件描述符列表
 */
int epoll_read_example(const char* filename) {
    int                epoll_fd, fd;
    struct epoll_event event, events[MAX_EVENTS];
    char               buffer[BUFFER_SIZE];
    ssize_t            bytes_read;

    printf("=== epoll 非阻塞读取示例 ===\n");

    // 创建 epoll 实例
    // epoll_create1() 创建一个新的 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        return -1;
    }

    // 打开文件
    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        close(epoll_fd);
        return -1;
    }

    // 设置为非阻塞模式
    if (set_nonblocking(fd) < 0) {
        close(fd);
        close(epoll_fd);
        return -1;
    }

    // 添加文件描述符到 epoll 实例
    // EPOLLIN: 监控可读事件
    event.events  = EPOLLIN;
    event.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
        perror("epoll_ctl EPOLL_CTL_ADD");
        close(fd);
        close(epoll_fd);
        return -1;
    }

    printf("等待文件可读...\n");

    // 等待事件
    // epoll_wait() 会阻塞，直到有事件发生
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (nfds < 0) {
        perror("epoll_wait");
        close(fd);
        close(epoll_fd);
        return -1;
    }

    printf("有 %d 个文件描述符就绪\n", nfds);

    // 处理就绪的文件描述符
    for (int i = 0; i < nfds; i++) {
        if (events[i].data.fd == fd) {
            // 非阻塞读取
            bytes_read = read(fd, buffer, BUFFER_SIZE);
            if (bytes_read < 0) {
                if (errno == EAGAIN || errno == EWOULDBLOCK) {
                    printf("暂时没有数据可读\n");
                } else {
                    perror("read");
                }
            } else if (bytes_read == 0) {
                printf("文件结束\n");
            } else {
                printf("读取 %zd 字节: %.*s\n", bytes_read, (int) bytes_read,
                    buffer);
            }
        }
    }

    // 从 epoll 实例中移除文件描述符
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);

    close(fd);
    close(epoll_fd);

    return 0;
}

/*
 * 使用 epoll 监控多个文件描述符
 *
 * 演示同时监控多个文件的可读事件
 */
int epoll_multi_file_example() {
    int                epoll_fd, fd1, fd2;
    struct epoll_event event, events[MAX_EVENTS];
    char               buffer[BUFFER_SIZE];
    ssize_t            bytes_read;

    printf("\n=== epoll 多文件监控示例 ===\n");

    // 创建 epoll 实例
    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        return -1;
    }

    // 打开第一个文件
    fd1 = open("standard_io_test.txt", O_RDONLY);
    if (fd1 < 0) {
        perror("open file1");
        close(epoll_fd);
        return -1;
    }
    set_nonblocking(fd1);

    // 打开第二个文件
    fd2 = open("buffered_io_test.txt", O_RDONLY);
    if (fd2 < 0) {
        perror("open file2");
        close(fd1);
        close(epoll_fd);
        return -1;
    }
    set_nonblocking(fd2);

    // 添加第一个文件到 epoll
    event.events  = EPOLLIN;
    event.data.fd = fd1;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd1, &event);

    // 添加第二个文件到 epoll
    event.events  = EPOLLIN;
    event.data.fd = fd2;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd2, &event);

    printf("等待多个文件可读...\n");

    // 等待事件
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (nfds < 0) {
        perror("epoll_wait");
        close(fd1);
        close(fd2);
        close(epoll_fd);
        return -1;
    }

    printf("有 %d 个文件描述符就绪\n", nfds);

    // 处理就绪的文件描述符
    for (int i = 0; i < nfds; i++) {
        int ready_fd = events[i].data.fd;
        bytes_read   = read(ready_fd, buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            if (ready_fd == fd1) {
                printf("文件1 读取 %zd 字节\n", bytes_read);
            } else if (ready_fd == fd2) {
                printf("文件2 读取 %zd 字节\n", bytes_read);
            }
        }
    }

    // 清理
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd1, NULL);
    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd2, NULL);
    close(fd1);
    close(fd2);
    close(epoll_fd);

    return 0;
}

/*
 * 使用 epoll 的边缘触发（Edge-Triggered）模式
 *
 * 边缘触发 vs 水平触发（Level-Triggered）：
 * - 水平触发（默认）：只要文件描述符就绪，就会一直通知
 * - 边缘触发：只在文件描述符状态变化时通知一次
 *
 * 边缘触发需要一次性读取所有数据，否则可能丢失事件
 */
int epoll_edge_triggered_example(const char* filename) {
    int                epoll_fd, fd;
    struct epoll_event event, events[MAX_EVENTS];
    char               buffer[BUFFER_SIZE];
    ssize_t            bytes_read;

    printf("\n=== epoll 边缘触发示例 ===\n");

    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        return -1;
    }

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        close(epoll_fd);
        return -1;
    }

    set_nonblocking(fd);

    // 使用边缘触发模式
    // EPOLLET: 边缘触发
    event.events  = EPOLLIN | EPOLLET;
    event.data.fd = fd;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event) < 0) {
        perror("epoll_ctl EPOLL_CTL_ADD");
        close(fd);
        close(epoll_fd);
        return -1;
    }

    printf("等待文件可读（边缘触发）...\n");

    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, -1);
    if (nfds < 0) {
        perror("epoll_wait");
        close(fd);
        close(epoll_fd);
        return -1;
    }

    // 边缘触发需要循环读取，直到 EAGAIN
    while (1) {
        bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read < 0) {
            if (errno == EAGAIN || errno == EWOULDBLOCK) {
                printf("读取完成（EAGAIN）\n");
                break;
            } else {
                perror("read");
                break;
            }
        } else if (bytes_read == 0) {
            printf("文件结束\n");
            break;
        } else {
            printf("读取 %zd 字节\n", bytes_read);
        }
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    close(epoll_fd);

    return 0;
}

/*
 * 使用 epoll 超时机制
 *
 * epoll_wait() 可以设置超时时间
 * 超时后返回 0，表示没有事件发生
 */
int epoll_timeout_example(const char* filename) {
    int                epoll_fd, fd;
    struct epoll_event event, events[MAX_EVENTS];
    char               buffer[BUFFER_SIZE];
    ssize_t            bytes_read;

    printf("\n=== epoll 超时示例 ===\n");

    epoll_fd = epoll_create1(0);
    if (epoll_fd < 0) {
        perror("epoll_create1");
        return -1;
    }

    fd = open(filename, O_RDONLY);
    if (fd < 0) {
        perror("open");
        close(epoll_fd);
        return -1;
    }

    set_nonblocking(fd);

    event.events  = EPOLLIN;
    event.data.fd = fd;
    epoll_ctl(epoll_fd, EPOLL_CTL_ADD, fd, &event);

    printf("等待文件可读（超时 2 秒）...\n");

    // 设置超时为 2 秒
    int nfds = epoll_wait(epoll_fd, events, MAX_EVENTS, 2000);
    if (nfds < 0) {
        perror("epoll_wait");
        close(fd);
        close(epoll_fd);
        return -1;
    } else if (nfds == 0) {
        printf("超时，没有事件发生\n");
    } else {
        printf("有 %d 个文件描述符就绪\n", nfds);
        bytes_read = read(fd, buffer, BUFFER_SIZE);
        if (bytes_read > 0) {
            printf("读取 %zd 字节\n", bytes_read);
        }
    }

    epoll_ctl(epoll_fd, EPOLL_CTL_DEL, fd, NULL);
    close(fd);
    close(epoll_fd);

    return 0;
}

int main() {
    // 先创建测试文件
    const char* filename = "epoll_test.txt";
    const char* data
        = "Hello, epoll!\nThis is a test file for epoll non-blocking I/O.\n";

    int fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        write(fd, data, strlen(data));
        fsync(fd);
        close(fd);
    }

    // epoll 非阻塞读取
    if (epoll_read_example(filename) < 0) {
        fprintf(stderr, "epoll 读取示例失败\n");
    }

    // epoll 多文件监控
    if (epoll_multi_file_example() < 0) {
        fprintf(stderr, "epoll 多文件示例失败\n");
    }

    // epoll 边缘触发
    if (epoll_edge_triggered_example(filename) < 0) {
        fprintf(stderr, "epoll 边缘触发示例失败\n");
    }

    // epoll 超时
    if (epoll_timeout_example(filename) < 0) {
        fprintf(stderr, "epoll 超时示例失败\n");
    }

    printf("\n所有 epoll 示例完成\n");
    return 0;
}
