# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm  

# Targets (programs to compile)
TARGETS = mercator mercator2 

# Default target
all: $(TARGETS)

# Rule to compile each program
mercator: mercator.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

mercator2: mercator2.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 

clean:
	rm -f $(TARGETS)