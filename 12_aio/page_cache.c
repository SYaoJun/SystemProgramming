/*
 * Page Cache 控制示例
 *
 * 演示三种不同的 I/O 方式：
 * 1. 标准 I/O（使用 page cache）+ fsync
 * 2. mmap（使用 page cache）+ msync
 * 3. O_DIRECT（绕过 page cache）
 */

#include <errno.h>
#include <fcntl.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/mman.h>
#include <sys/stat.h>
#include <unistd.h>

#define FILE_SIZE 4096
#define BUFFER_SIZE 4096

/*
 * 方式1: 标准 I/O 使用 Page Cache
 *
 * 标准 write() 调用会先将数据写入内核的 page cache，
 * 然后由内核异步刷新到磁盘。
 *
 * fsync() 强制将 page cache 中的数据立即刷新到磁盘。
 */
int standard_io_with_fsync(const char* filename) {
    int     fd;
    char    buffer[BUFFER_SIZE] = "Standard I/O with page cache and fsync";
    ssize_t bytes_written;

    printf("\n=== 方式1: 标准 I/O + fsync ===\n");

    // 打开文件，O_TRUNC 清空文件
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    // 写入数据 - 此时数据先进入 page cache
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
        perror("write");
        close(fd);
        return -1;
    }

    printf("数据已写入 page cache (大小: %zd 字节)\n", bytes_written);

    // fsync() 强制将 page cache 中的数据刷新到磁盘
    // 这会阻塞直到数据真正写入磁盘
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
 * 方式2: mmap 使用 Page Cache
 *
 * mmap() 将文件映射到内存空间，访问映射的内存会自动使用 page cache。
 *
 * msync() 强制将映射内存中的修改刷新到磁盘。
 */
int mmap_with_msync(const char* filename) {
    int         fd;
    char*       mapped;
    struct stat sb;

    printf("\n=== 方式2: mmap + msync ===\n");

    // 打开文件
    fd = open(filename, O_RDWR | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    // 设置文件大小
    if (ftruncate(fd, FILE_SIZE) < 0) {
        perror("ftruncate");
        close(fd);
        return -1;
    }

    // mmap 映射文件到内存
    // MAP_SHARED 表示修改会写回文件，使用 page cache
    mapped = mmap(NULL, FILE_SIZE, PROT_READ | PROT_WRITE, MAP_SHARED, fd, 0);
    if (mapped == MAP_FAILED) {
        perror("mmap");
        close(fd);
        return -1;
    }

    printf("文件已映射到内存 (地址: %p)\n", mapped);

    // 修改映射的内存 - 修改会进入 page cache
    strcpy(mapped, "mmap with page cache and msync");
    printf("数据已写入映射内存 (page cache)\n");

    // msync() 强制将修改刷新到磁盘
    // MS_SYNC 同步刷新，阻塞直到完成
    if (msync(mapped, FILE_SIZE, MS_SYNC) < 0) {
        perror("msync");
        munmap(mapped, FILE_SIZE);
        close(fd);
        return -1;
    }

    printf("msync 完成，数据已刷新到磁盘\n");

    // 解除映射
    if (munmap(mapped, FILE_SIZE) < 0) {
        perror("munmap");
    }

    close(fd);
    return 0;
}

/*
 * 方式3: O_DIRECT 绕过 Page Cache
 *
 * O_DIRECT 标志绕过 page cache，数据直接在用户空间和磁盘之间传输。
 *
 * 注意：使用 O_DIRECT 有以下限制：
 * 1. 缓冲区地址必须对齐（通常是 512 字节）
 * 2. 文件偏移必须对齐
 * 3. 写入大小必须对齐
 * 4. 不保证数据顺序
 *
 * 即使使用 O_DIRECT，仍建议使用 fsync 确保数据持久化。
 */
int o_direct_io(const char* filename) {
    int     fd;
    void*   buffer;
    ssize_t bytes_written;

    printf("\n=== 方式3: O_DIRECT (绕过 page cache) ===\n");

    // 分配对齐的缓冲区（页面对齐，通常是 4096 字节）
    if (posix_memalign(&buffer, 4096, BUFFER_SIZE) != 0) {
        perror("posix_memalign");
        return -1;
    }

    strcpy(buffer, "O_DIRECT bypasses page cache");

    // 打开文件，使用 O_DIRECT 标志
    // 注意：O_DIRECT 需要 O_RDWR 或 O_WRONLY
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_DIRECT, 0644);
    if (fd < 0) {
        perror("open with O_DIRECT");
        free(buffer);
        return -1;
    }

    printf("文件已用 O_DIRECT 打开\n");

    // 写入数据 - 直接传输到磁盘，不经过 page cache
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
        perror("write with O_DIRECT");
        close(fd);
        free(buffer);
        return -1;
    }

    printf("数据已直接写入磁盘 (绕过 page cache, 大小: %zd 字节)\n",
        bytes_written);

    // 即使使用 O_DIRECT，仍建议使用 fsync 确保数据真正持久化
    if (fsync(fd) < 0) {
        perror("fsync");
        close(fd);
        free(buffer);
        return -1;
    }

    printf("fsync 完成，确保数据持久化\n");

    close(fd);
    free(buffer);
    return 0;
}

/*
 * 方式4: fdatasync - 仅刷新数据，不刷新元数据
 *
 * fdatasync() 类似于 fsync()，但只刷新文件数据，
 * 不刷新文件元数据（如访问时间、修改时间等）。
 *
 * 在某些情况下，fdatasync 比 fsync 更快。
 */
int standard_io_with_fdatasync(const char* filename) {
    int     fd;
    char    buffer[BUFFER_SIZE] = "Standard I/O with page cache and fdatasync";
    ssize_t bytes_written;

    printf("\n=== 方式4: 标准 I/O + fdatasync ===\n");

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
        perror("write");
        close(fd);
        return -1;
    }

    printf("数据已写入 page cache (大小: %zd 字节)\n", bytes_written);

    // fdatasync() 仅刷新数据，不刷新元数据
    if (fdatasync(fd) < 0) {
        perror("fdatasync");
        close(fd);
        return -1;
    }

    printf("fdatasync 完成，数据已刷新到磁盘（元数据未刷新）\n");

    close(fd);
    return 0;
}

/*
 * 方式5: O_SYNC - 同步写入
 *
 * O_SYNC 标志使每次 write() 调用都同步等待数据写入磁盘。
 *
 * 这相当于每次 write() 后自动调用 fsync()，但性能较差。
 */
int o_sync_io(const char* filename) {
    int     fd;
    char    buffer[BUFFER_SIZE] = "O_SYNC synchronous write";
    ssize_t bytes_written;

    printf("\n=== 方式5: O_SYNC (同步写入) ===\n");

    // 使用 O_SYNC 标志
    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC | O_SYNC, 0644);
    if (fd < 0) {
        perror("open with O_SYNC");
        return -1;
    }

    printf("文件已用 O_SYNC 打开\n");

    // 每次 write() 都会阻塞直到数据写入磁盘
    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
        perror("write with O_SYNC");
        close(fd);
        return -1;
    }

    printf("数据已同步写入磁盘 (大小: %zd 字节)\n", bytes_written);

    close(fd);
    return 0;
}

/*
 * 方式6: sync_file_range - 精确控制刷新范围
 *
 * sync_file_range() 允许精确控制刷新文件的哪些部分。
 *
 * 注意：这是 Linux 特有的系统调用。
 */
#ifdef __linux__
int sync_file_range_example(const char* filename) {
    int     fd;
    char    buffer[BUFFER_SIZE] = "sync_file_range example";
    ssize_t bytes_written;

    printf("\n=== 方式6: sync_file_range (Linux 特有) ===\n");

    fd = open(filename, O_WRONLY | O_CREAT | O_TRUNC, 0644);
    if (fd < 0) {
        perror("open");
        return -1;
    }

    bytes_written = write(fd, buffer, strlen(buffer));
    if (bytes_written < 0) {
        perror("write");
        close(fd);
        return -1;
    }

    printf("数据已写入 page cache\n");

    // SYNC_FILE_RANGE_WAIT_BEFORE: 等待之前的写入完成
    // SYNC_FILE_RANGE_WRITE: 启动写入
    // SYNC_FILE_RANGE_WAIT_AFTER: 等待写入完成
    if (sync_file_range(fd, 0, bytes_written,
            SYNC_FILE_RANGE_WAIT_BEFORE | SYNC_FILE_RANGE_WRITE
                | SYNC_FILE_RANGE_WAIT_AFTER)
        < 0) {
        perror("sync_file_range");
        close(fd);
        return -1;
    }

    printf("sync_file_range 完成，指定范围已刷新\n");

    close(fd);
    return 0;
}
#endif

int main(int argc, char* argv[]) {
    const char* filename = "page_cache_test.txt";

    printf("Page Cache 控制示例\n");
    printf("====================\n");

    // 方式1: 标准 I/O + fsync
    if (standard_io_with_fsync(filename) < 0) {
        fprintf(stderr, "标准 I/O 测试失败\n");
    }

    // 方式2: mmap + msync
    if (mmap_with_msync(filename) < 0) {
        fprintf(stderr, "mmap 测试失败\n");
    }

    // 方式3: O_DIRECT
    if (o_direct_io(filename) < 0) {
        fprintf(stderr, "O_DIRECT 测试失败\n");
        printf("注意: O_DIRECT 可能需要 root 权限或特定文件系统支持\n");
    }

    // 方式4: 标准 I/O + fdatasync
    if (standard_io_with_fdatasync(filename) < 0) {
        fprintf(stderr, "fdatasync 测试失败\n");
    }

    // 方式5: O_SYNC
    if (o_sync_io(filename) < 0) {
        fprintf(stderr, "O_SYNC 测试失败\n");
    }

#ifdef __linux__
    // 方式6: sync_file_range (仅 Linux)
    if (sync_file_range_example(filename) < 0) {
        fprintf(stderr, "sync_file_range 测试失败\n");
    }
#endif

    printf("\n所有测试完成\n");
    printf("测试文件: %s\n", filename);

    return 0;
}
