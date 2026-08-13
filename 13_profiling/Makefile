# Compiler and linker
CC = gcc

# Source files
SRC = main.c

# Output executable
TARGET = main

# Compiler flags
CFLAGS = -pg -no-pie -fno-builtin -O0 -fprofile-arcs -ftest-coverage

# Linker flags
LDFLAGS = -lgcov

# Build target
$(TARGET): $(SRC)
	$(CC) $(SRC) -o $(TARGET) $(CFLAGS) $(LDFLAGS)

# Clean target
clean:
	rm -f $(TARGET) *.gcda *.gcno *.gcov *.out
