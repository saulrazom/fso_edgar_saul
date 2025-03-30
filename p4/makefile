# Compiler and flags
CC = gcc
CFLAGS = -Wall -Wextra
LDFLAGS = -lm -lrt  # Agregamos -lrt para colas POSIX

# Targets (programs to compile)
TARGETS = mercator_original mercator_corregido

# Default target
all: $(TARGETS)

# Reglas de compilación
mercator_original: mercator_original.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

mercator_corregido: mercator_corregido.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

# Limpieza
clean:
	rm -f $(TARGETS)

# Evitar conflictos con archivos con nombres similares
.PHONY: all clean