# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra

# Targets (programs to compile)
TARGETS = init getty sh 

# Default target
all: $(TARGETS)

# Rule to compile each program
init: init.c
	$(CC) $(CFLAGS) -o $@ $<

getty: getty.c
	$(CC) $(CFLAGS) -o $@ $<

sh: sh.c
	$(CC) $(CFLAGS) -o $@ $<


clean:
	rm -f $(TARGETS)
