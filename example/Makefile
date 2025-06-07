# Compiler
CC = gcc

# Compiler flags
CFLAGS = -I../include -Wall -Wextra -g

# Source files
SRCS = eg.c ../src/stack_alloc.c

# Output binary
TARGET = demo

# Default target
all: $(TARGET)

# Link object files
$(TARGET): $(SRCS)
	$(CC) $(CFLAGS) $(SRCS) -o $(TARGET)

# Clean build files
clean:
	rm -f $(TARGET)
