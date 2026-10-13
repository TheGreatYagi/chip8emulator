# 1. Compiler and Build Arguments (Flags)
CC      = clang
CFLAGS  = -Wall -Wextra -g -O2 -std=c11 -I/opt/homebrew/include 
LDFLAGS = -L/opt/homebrew/lib -lSDL2

# 2. Source Files & Object Files
# Header files (.h) are listed as dependencies so changes trigger a recompile
HEADERS = chip8.h cpu.h display.h 
SOURCES = main.c cpu.c display.c chip8.c
OBJECTS = $(SOURCES:.c=.o)

# 3. Target Executable
TARGET  = ch8mu

# Default target
all: $(TARGET)

# Linking the target executable
$(TARGET): $(OBJECTS)
	$(CC) $(OBJECTS) -o $(TARGET) $(LDFLAGS)

# Compiling source files into object files
# Including $(HEADERS) ensures files recompile if a header changes
%.o: %.c $(HEADERS)
	$(CC) $(CFLAGS) -c $< -o $@

# Clean up build artifacts
clean:
	rm -f $(OBJECTS)

.PHONY: all clean%