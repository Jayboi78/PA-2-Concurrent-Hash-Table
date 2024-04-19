# Compiler
CC = gcc

# Compiler flags
CFLAGS = -Wall -Wextra -pthread

# Source files
SRCS = chash.c hashdb.c rwlocks.c

# Object files
OBJS = $(SRCS:.c=.o)

# Executable name
TARGET = chash

# Default rule
all: $(TARGET)

# Compile source files into object files
%.o: %.c
    $(CC) $(CFLAGS) -c $< -o $@

# Link object files into executable
$(TARGET): $(OBJS)
    $(CC) $(CFLAGS) $(OBJS) -o $@

# Clean rule
clean:
    rm -f $(OBJS) $(TARGET)
