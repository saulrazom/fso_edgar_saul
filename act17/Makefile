# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm  

# Targets (programs to compile)
TARGETS = act17

# Default target
all: $(TARGETS)

# Rule to compile each program
act17: act17.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

clean:
	rm -f $(TARGETS)