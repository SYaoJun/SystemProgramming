# 指定编译器
CC = gcc

# 获取当前目录下的所有C文件
SRCS = $(wildcard *.c)

# 将所有C文件转换为目标文件名
TARGETS = $(SRCS:.c=)

# 特定文件的编译选项
libaio_demo: LDFLAGS += -laio
io_uring_1: LDFLAGS += -luring

# 默认目标是所有的可执行文件
all: $(TARGETS)

# 规则：将.c文件编译为可执行文件
%: %.c
	$(CC) -o $@ $< $(LDFLAGS)

# 清理生成的可执行文件
clean:
	rm -f $(TARGETS)

.PHONY: all clean
