# Makefile for Ring Buffer Example Project
# -----------------------------------------------------------------------------
# This Makefile compiles the ring buffer implementation along with the main
# application that demonstrates enqueueing and dequeueing operations using
# threads and mutexes.

# Compiler to use
CC := gcc

# Compiler flags
# -std=c11      : Use the C11 standard
# -Wall -Wextra : Enable all warnings for better code quality
# -pthread      : Enable POSIX thread support
CFLAGS := -std=c11 -Wall -Wextra -pthread

# Linker flags
# -pthread      : Ensure thread libraries are linked
LDFLAGS := -pthread

# Source files
SOURCES := main.c ring_buffer.c

# Object files (replace .c with .o)
OBJECTS := $(SOURCES:.c=.o)

# Executable name
TARGET := example.exe

# Default target that builds the executable
all: $(TARGET)

# Rule to build the executable by linking object files
$(TARGET): $(OBJECTS)
	@echo "Linking object files to create executable..."
	$(CC) $(CFLAGS) $(OBJECTS) -o $(TARGET) $(LDFLAGS)
	@echo "Build complete: $(TARGET)"

# Generic rule to compile .c files to .o object files
%.o: %.c ring_buffer.h
	@echo "Compiling $<..."
	$(CC) $(CFLAGS) -c $< -o $@

# Clean target to remove compiled object files and executable
clean:
	@echo "Cleaning up build artifacts..."
	rm -f $(OBJECTS) $(TARGET)
	@echo "Clean complete."

# Phony targets to prevent conflicts with files named 'all' or 'clean'
.PHONY: all clean
