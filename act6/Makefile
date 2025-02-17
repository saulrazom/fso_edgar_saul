# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Targets (programs to compile)
TARGETS = ejer1 matprimos

# Default target
all: $(TARGETS)

# Rule to compile each program
ejer1: ejer1.c
	$(CC) $(CFLAGS) -o $@ $<

matprimos: matprimos.c
	$(CC) $(CFLAGS) -o $@ $<

clean:
	rm -f $(TARGETS)
