/*
 * sendfile 零拷贝示例
 *
 * 演示使用 sendfile() 系统调用进行零拷贝文件传输
 *
 * sendfile() 在内核空间直接将文件数据从一个文件描述符复制到另一个
 * 避免了数据在用户空间和内核空间之间的来回拷贝
 *
 * 适用于文件传输、网络服务器等场景，提高性能
 */

#include <arpa/inet.h>
#include <errno.h>
#include <fcntl.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/sendfile.h>
#include <sys/socket.h>
#include <sys/stat.h>
#include <unistd.h>

#define BUFFER_SIZE 4096
#define PORT 8888

/*
 * 使用 sendfile() 在两个文件之间传输数据
 *
 * sendfile() 直接在内核空间完成数据传输
 * 不需要经过用户空间，实现零拷贝
 */
int sendfile_file_to_file(const char* src_file, const char* dst_file) {
    int         src_fd, dst_fd;
    struct stat stat_buf;
    off_t       offset = 0;
    ssize_t     sent_bytes;

    printf("=== sendfile 文件到文件传输示例 ===\n");

    // 打开源文件
    src_fd = open(src_file, O_RDONLY);
    if (src_fd < 0) {
        perror("open src file");
        return -1;
    }

    // 获取文件大小
    if (fstat(src_fd, &stat_buf) < 0) {
        perror("fstat");
        close(src_fd);
        return -1;
    }

    printf("源文件大小: %ld 字节\n", stat_buf.st_size);

    // 打开目标文件
    dst_fd = open(dst_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        perror("open dst file");
        close(src_fd);
        return -1;
    }

    // 使用 sendfile 传输数据
    // sendfile() 在内核空间直接传输数据
    // 参数：目标文件描述符，源文件描述符，偏移量，传输字节数
    sent_bytes = sendfile(dst_fd, src_fd, &offset, stat_buf.st_size);
    if (sent_bytes < 0) {
        perror("sendfile");
        close(src_fd);
        close(dst_fd);
        return -1;
    }

    printf("sendfile 传输了 %zd 字节\n", sent_bytes);

    // fsync 确保数据写入磁盘
    if (fsync(dst_fd) < 0) {
        perror("fsync");
        close(src_fd);
        close(dst_fd);
        return -1;
    }

    printf("fsync 完成\n");

    close(src_fd);
    close(dst_fd);

    return 0;
}

/*
 * 使用 sendfile() 从文件传输到 socket
 *
 * 这是 sendfile 的典型应用场景：文件服务器
 * 直接将文件数据发送到网络 socket，无需经过用户空间
 */
int sendfile_file_to_socket(const char* filename) {
    int                server_fd, client_fd, file_fd;
    struct sockaddr_in server_addr, client_addr;
    socklen_t          client_len = sizeof(client_addr);
    struct stat        stat_buf;
    off_t              offset = 0;
    ssize_t            sent_bytes;

    printf("\n=== sendfile 文件到 Socket 传输示例 ===\n");

    // 创建 TCP socket
    server_fd = socket(AF_INET, SOCK_STREAM, 0);
    if (server_fd < 0) {
        perror("socket");
        return -1;
    }

    // 设置地址重用
    int opt = 1;
    setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    // 绑定地址和端口
    memset(&server_addr, 0, sizeof(server_addr));
    server_addr.sin_family      = AF_INET;
    server_addr.sin_addr.s_addr = INADDR_ANY;
    server_addr.sin_port        = htons(PORT);

    if (bind(server_fd, (struct sockaddr*) &server_addr, sizeof(server_addr))
        < 0) {
        perror("bind");
        close(server_fd);
        return -1;
    }

    // 监听连接
    if (listen(server_fd, 1) < 0) {
        perror("listen");
        close(server_fd);
        return -1;
    }

    printf("服务器监听端口 %d\n", PORT);
    printf("等待客户端连接...\n");

    // 接受客户端连接
    client_fd = accept(server_fd, (struct sockaddr*) &client_addr, &client_len);
    if (client_fd < 0) {
        perror("accept");
        close(server_fd);
        return -1;
    }

    printf("客户端已连接: %s:%d\n", inet_ntoa(client_addr.sin_addr),
        ntohs(client_addr.sin_port));

    // 打开要传输的文件
    file_fd = open(filename, O_RDONLY);
    if (file_fd < 0) {
        perror("open file");
        close(client_fd);
        close(server_fd);
        return -1;
    }

    // 获取文件大小
    if (fstat(file_fd, &stat_buf) < 0) {
        perror("fstat");
        close(file_fd);
        close(client_fd);
        close(server_fd);
        return -1;
    }

    printf("文件大小: %ld 字节\n", stat_buf.st_size);

    // 使用 sendfile 将文件数据发送到 socket
    sent_bytes = sendfile(client_fd, file_fd, &offset, stat_buf.st_size);
    if (sent_bytes < 0) {
        perror("sendfile");
        close(file_fd);
        close(client_fd);
        close(server_fd);
        return -1;
    }

    printf("sendfile 传输了 %zd 字节到客户端\n", sent_bytes);

    close(file_fd);
    close(client_fd);
    close(server_fd);

    return 0;
}

/*
 * 分块使用 sendfile 传输大文件
 *
 * 对于大文件，可以分块传输，避免一次性传输过多数据
 */
int sendfile_chunked(const char* src_file, const char* dst_file) {
    int         src_fd, dst_fd;
    struct stat stat_buf;
    off_t       offset = 0;
    ssize_t     sent_bytes;
    size_t      chunk_size = 1024 * 1024; // 1MB chunks
    size_t      total_sent = 0;

    printf("\n=== sendfile 分块传输示例 ===\n");

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

    printf("源文件大小: %ld 字节\n", stat_buf.st_size);

    dst_fd = open(dst_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (dst_fd < 0) {
        perror("open dst file");
        close(src_fd);
        return -1;
    }

    // 分块传输
    while (offset < stat_buf.st_size) {
        size_t to_send = chunk_size;
        if (stat_buf.st_size - offset < chunk_size) {
            to_send = stat_buf.st_size - offset;
        }

        sent_bytes = sendfile(dst_fd, src_fd, &offset, to_send);
        if (sent_bytes < 0) {
            perror("sendfile");
            close(src_fd);
            close(dst_fd);
            return -1;
        }

        total_sent += sent_bytes;
        printf("已传输 %zu / %ld 字节 (%.1f%%)\n", total_sent, stat_buf.st_size,
            (double) total_sent / stat_buf.st_size * 100);
    }

    printf("分块传输完成，总共 %zu 字节\n", total_sent);

    if (fsync(dst_fd) < 0) {
        perror("fsync");
        close(src_fd);
        close(dst_fd);
        return -1;
    }

    close(src_fd);
    close(dst_fd);

    return 0;
}

/*
 * 对比传统 read/write 和 sendfile 的性能
 *
 * 传统方式：read() 将数据从内核读到用户空间，write() 再写回内核
 * sendfile：直接在内核空间传输，零拷贝
 */
int compare_performance(const char* src_file, const char* dst_file_traditional,
    const char* dst_file_sendfile) {
    int         src_fd, dst_fd;
    struct stat stat_buf;
    char        buffer[BUFFER_SIZE];
    ssize_t     bytes_read, bytes_written;
    off_t       offset = 0;

    printf("\n=== 性能对比示例 ===\n");

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

    // sendfile 方式
    src_fd = open(src_file, O_RDONLY);
    dst_fd = open(dst_file_sendfile, O_WRONLY | O_CREAT | O_TRUNC, 0644);

    printf("使用 sendfile 方式...\n");
    offset = 0;
    sendfile(dst_fd, src_fd, &offset, stat_buf.st_size);

    fsync(dst_fd);
    close(dst_fd);
    close(src_fd);

    printf("性能对比完成\n");
    printf("传统方式：数据在用户空间和内核空间之间来回拷贝\n");
    printf("sendfile：零拷贝，直接在内核空间传输\n");

    return 0;
}

int main() {
    const char* src_file        = "sendfile_src.txt";
    const char* dst_file        = "sendfile_dst.txt";
    const char* dst_chunked     = "sendfile_chunked.txt";
    const char* dst_traditional = "sendfile_traditional.txt";
    const char* dst_sendfile    = "sendfile_sendfile.txt";

    // 创建测试文件
    int fd = open(src_file, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd >= 0) {
        const char* data
            = "Hello, sendfile!\nThis is a test file for zero-copy file "
              "transfer.\n"
              "sendfile is efficient for file transfer and network servers.\n";
        for (int i = 0; i < 1000; i++) {
            write(fd, data, strlen(data));
        }
        fsync(fd);
        close(fd);
    }

    // sendfile 文件到文件
    if (sendfile_file_to_file(src_file, dst_file) < 0) {
        fprintf(stderr, "sendfile 文件到文件失败\n");
    }

    // sendfile 分块传输
    if (sendfile_chunked(src_file, dst_chunked) < 0) {
        fprintf(stderr, "sendfile 分块传输失败\n");
    }

    // 性能对比
    if (compare_performance(src_file, dst_traditional, dst_sendfile) < 0) {
        fprintf(stderr, "性能对比失败\n");
    }

    // sendfile 到 socket（需要手动测试）
    printf("\n注意：sendfile 到 socket 示例需要手动测试\n");
    printf("可以取消注释 main() 中的 sendfile_file_to_socket() 调用\n");

    printf("\n所有 sendfile 示例完成\n");
    return 0;
}
