# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Targets (programs to compile)
TARGETS = dos

# Default target
all: $(TARGETS)

# Rule to compile each program
dos: dos.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
