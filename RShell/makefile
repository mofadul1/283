# Compiler settings
CC = gcc
CFLAGS = -Wall -Wextra -g

# Target executable name
TARGET = dsh

# Find all source and header files
SRCS = dsh_cli.c dshlib.c rsh_cli.c rsh_server.c
HDRS = dshlib.h rshlib.h

# Default target
all: $(TARGET)

# Compile source to executable
$(TARGET): $(SRCS) $(HDRS)
	$(CC) $(CFLAGS) -o $(TARGET) $(SRCS)

# Clean up build files
clean:
	rm -f $(TARGET)

test:
	bats $(wildcard ./bats/*.sh)

valgrind:
	echo "pwd\nexit" | valgrind --leak-check=full --show-leak-kinds=all --error-exitcode=1 ./$(TARGET) 
	echo "pwd\nexit" | valgrind --tool=helgrind --error-exitcode=1 ./$(TARGET) 

# Phony targets
.PHONY: all clean test
