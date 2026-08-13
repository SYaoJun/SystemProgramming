# 静态库和动态库联合编译
```
make libadd.a
make libsub.so
```
## 多个目标文件
放到all后面
## 目标文件之间有依赖
将依赖的目标放置在target的冒号后面即可，编译时会优先编译依赖的目标。
## 在makefile中打印日志
```c
$(TARGET2): $(TARGET1)
	@echo "Building $(TARGET2) which depends on $(TARGET1)"
	$(CC) $(CFLAGS) $(SRC) -o $@
```
## 静态库
ar rcs libmystatic.a obj1.o obj2.o obj3.o：使用 ar 工具创建一个名为 libmystatic.a 的静态库，包含前面编译生成的对象文件。
r：插入文件（替换旧版本）。
c：不显示警告信息。
s：创建索引信息。