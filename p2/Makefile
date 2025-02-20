# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Targets (programs to compile)
TARGETS = serial parallel

# Default target
all: $(TARGETS)

# Rule to compile each program
serial: serial.c
	$(CC) $(CFLAGS) -o $@ $<

parallel: parallel.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
