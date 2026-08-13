# Basics

- `hello.cpp` - Bazel + C++ hello world
- `calloc.c` - malloc vs calloc
- `check_endian.c` - check endianness via pointer
- `little_endian.c` - determine endianness via pointer
- `union_usage.c` - union usage

## Build

```bash
# build all targets
bazel build //00_basic:all

# build a specific target
bazel build //00_basic:hello
```

## Run

```bash
bazel run //00_basic:hello
```
