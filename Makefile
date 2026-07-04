CC = gcc
CFLAGS = -Wall -Wextra -pedantic

# Optimization flags: O3 for speed, march=native for specific CPU, -s to strip bloat
OPT_FLAGS = -O3 -march=native -s

# Debug flags: -g for symbols, -O0 to disable optimizations
DEBUG_FLAGS = -g -O0

.PHONY: all debug clean

# Default target
all: tedi

tedi: tedi.c
	@mkdir -p build
	$(CC) tedi.c $(CFLAGS) $(OPT_FLAGS) -o build/tedi
	@echo "Built optimized release binary in build/tedi"

debug: tedi.c
	@mkdir -p build
	$(CC) tedi.c $(CFLAGS) $(DEBUG_FLAGS) -o build/tedi_debug
	@echo "Built debug binary in build/tedi_debug"

clean:
	rm -rf build/tedi build/tedi_debug
	@echo "Cleaned build directory"
