# Variables
CC = gcc  # Compiler to use
CFLAGS = -Wall -Wextra -O2 -pthread  # Compilation flags (adjust as necessary)
LDFLAGS = -pthread  # Linker flags for thread support

# Source files
SRCS = chash.c hashdb.c rwlocks.c

# Object files
OBJS = $(SRCS:.c=.o)

# Header files
HDRS = hashdb.h rwlocks.h common.h common_threads.h

# Output executable
TARGET = chash

# Default target
all: $(TARGET)

# Compile source files into object files
%.o: %.c $(HDRS)
	$(CC) $(CFLAGS) -c $< -o $@

# Link object files to create the executable
$(TARGET): $(OBJS)
	$(CC) $(OBJS) -o $(TARGET) $(LDFLAGS)

# Clean target to remove compiled files
clean:
	rm -f $(OBJS) $(TARGET)
