# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm  

# Targets (programs to compile)
TARGETS = mercator mercator_sem

# Default target
all: $(TARGETS)

# Rule to compile each program
mercator: mercator.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

mercator_sem: mercator_sem.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

clean:
	rm -f $(TARGETS)