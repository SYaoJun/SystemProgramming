# 定义编译器
CC := gcc

# 定义编译选项
CFLAGS := -Wall -g

# 定义源文件
SRC := add.c 

# 定义目标文件
OBJ := $(SRC:.c=.o)

# 定义静态库名称
LIB := libadd.a
DLIB = libsub.so 

# 默认目标
all: $(LIB) $(DLIB)

# 创建静态库
$(LIB): $(OBJ)
	$(AR) rcs $@ $^

# 编译源文件生成目标文件
%.o: %.c
	$(CC) $(CFLAGS) -c $< -o $@

# 清理生成的文件
clean:
	rm -f $(OBJ) $(LIB) $(DLIB)

# 包含依赖
-include $(OBJ:.o=.d)

libsub.so: sub.c $(LIB)
	gcc -shared -fPIC -o libsub.so sub.c -ladd -L. 