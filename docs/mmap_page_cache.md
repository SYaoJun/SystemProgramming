# mmap 与 Page Cache 的关系

## 问题：mmap 是否使用 page cache？munmap 是否自动写回磁盘？

### 核心概念

**mmap 确实使用 page cache**，这是关键点：

1. **mmap 不是直接的"文件-内存共享"**
   - mmap 通过 page cache 作为中介层
   - 文件内容先被加载到 page cache
   - mmap 的虚拟内存地址映射到 page cache 的物理页面
   - 修改 mmap 区域 = 修改 page cache

2. **munmap 不保证立即写回**
   - `munmap()` 只是解除虚拟内存映射
   - 数据仍在 page cache 中
   - 内核会在之后的某个时间点异步刷新到磁盘
   - **不保证何时刷新**，可能延迟很久

3. **为什么需要 msync？**
   - `msync()` 强制将 page cache 中的修改立即刷新到磁盘
   - 保证数据持久化
   - 类似于普通文件的 `fsync()`

### 类比对比

| 方式 | 写入路径 | 刷新机制 |
|------|---------|---------|
| 标准 I/O | `write()` → page cache → `fsync()` → 磁盘 | `fsync()` 强制刷新 |
| mmap | 修改映射内存 → page cache → `msync()` → 磁盘 | `msync()` 强制刷新 |

### 代码示例

```c
// 错误做法：只 munmap，不保证写回
munmap(mapped, size);  // 只是解除映射，不保证写回
// 此时数据还在 page cache，可能几秒后才刷新

// 正确做法：先 msync，再 munmap
msync(mapped, size, MS_SYNC);  // 先强制刷新
munmap(mapped, size);          // 再解除映射
```

### MAP_SHARED vs MAP_PRIVATE

- **MAP_SHARED**: 修改会写回文件，使用 page cache
- **MAP_PRIVATE**: 创建写时复制（COW）的私有映射，修改不会写回文件

### 总结

- mmap 通过 page cache 管理文件内容
- munmap 不保证数据立即写回磁盘
- 需要使用 msync() 强制刷新，确保数据持久化
- 这与标准 I/O 中 write() + fsync() 的模式类似
