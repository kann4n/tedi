# Default compiler and flags
CC      ?= gcc
CSTD    ?= -std=c11
CFLAGS  += $(CSTD) -Wall -Wextra -Wpedantic -I$(SRC_DIR)

# Directory layout
SRC_DIR   := src
BUILD_DIR := build

# Automatically discover all C files in src/
SRC := $(wildcard $(SRC_DIR)/*.c)
OBJ := $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRC))
DEP := $(OBJ:.o=.d)

# Output binaries
TARGET_RELEASE := $(BUILD_DIR)/tedi
TARGET_DEBUG   := $(BUILD_DIR)/tedi_debug

# Optimization and Debug Flags
OPT_FLAGS   := -O3 -march=native -s
DEBUG_FLAGS := -g -O0

.PHONY: all debug clean

# Default target: release build
all: CFLAGS += $(OPT_FLAGS)
all: $(TARGET_RELEASE)

# Debug target
debug: CFLAGS += $(DEBUG_FLAGS)
debug: $(TARGET_DEBUG)

# Link release executable
$(TARGET_RELEASE): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Built release binary: $@"

# Link debug executable
$(TARGET_DEBUG): $(OBJ)
	$(CC) $(CFLAGS) $^ -o $@
	@echo "Built debug binary: $@"

# Compile .c source files into .o object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	@mkdir -p $(BUILD_DIR)
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

# Include header dependencies
-include $(DEP)

# Clean build directory
clean:
	rm -rf $(BUILD_DIR)
	@echo "Cleaned build directory"
