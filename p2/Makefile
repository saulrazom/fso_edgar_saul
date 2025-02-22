# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm  

# Targets (programs to compile)
TARGETS = serial parallel

# Default target
all: $(TARGETS)

# Rule to compile each program
serial: serial.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) 


parallel: parallel.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS) -pthread  

clean:
	rm -f $(TARGETS)