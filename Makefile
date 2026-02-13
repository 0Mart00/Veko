CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -shared
MATH_LIB = -lm

# Directories
BUILD_DIR = build
CORE_DIR = modules/core
MATH_DIR = modules/math
OOP_DIR = modules/oop
BUILTINS_DIR = modules/builtins

# Source files
CORE_SRCS = $(CORE_DIR)/memory.c
MATH_SRCS = $(MATH_DIR)/math_module.c
OOP_SRCS = $(OOP_DIR)/class_system.c
BUILTINS_SRCS = $(BUILTINS_DIR)/builtins.c
MAIN_SRC = modules/logic.c

# Object files
CORE_OBJS = $(BUILD_DIR)/memory.o
MATH_OBJS = $(BUILD_DIR)/math_module.o
OOP_OBJS = $(BUILD_DIR)/class_system.o
BUILTINS_OBJS = $(BUILD_DIR)/builtins.o
MAIN_OBJ = $(BUILD_DIR)/logic_main.o

ALL_OBJS = $(CORE_OBJS) $(MATH_OBJS) $(OOP_OBJS) $(BUILTINS_OBJS) $(MAIN_OBJ)

# Targets
all: setup $(BUILD_DIR)/engine_host $(BUILD_DIR)/logic.so

setup:
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/engine_host: core/main.c
	$(CC) core/main.c -o $(BUILD_DIR)/engine_host -ldl

# Core module
$(BUILD_DIR)/memory.o: $(CORE_SRCS)
	$(CC) $(CFLAGS) -c $(CORE_SRCS) -o $(BUILD_DIR)/memory.o

# Math module
$(BUILD_DIR)/math_module.o: $(MATH_SRCS)
	$(CC) $(CFLAGS) -c $(MATH_SRCS) -o $(BUILD_DIR)/math_module.o $(MATH_LIB)

# OOP module
$(BUILD_DIR)/class_system.o: $(OOP_SRCS)
	$(CC) $(CFLAGS) -c $(OOP_SRCS) -o $(BUILD_DIR)/class_system.o

# Builtins module
$(BUILD_DIR)/builtins.o: $(BUILTINS_SRCS)
	$(CC) $(CFLAGS) -c $(BUILTINS_SRCS) -o $(BUILD_DIR)/builtins.o $(MATH_LIB)

# Main logic
$(BUILD_DIR)/logic_main.o: $(MAIN_SRC)
	$(CC) $(CFLAGS) -c $(MAIN_SRC) -o $(BUILD_DIR)/logic_main.o

# Link all modules into shared library
$(BUILD_DIR)/logic.so: $(ALL_OBJS)
	$(CC) $(LDFLAGS) $(ALL_OBJS) -o $(BUILD_DIR)/logic.so $(MATH_LIB)

clean:
	rm -rf $(BUILD_DIR)/*

module: $(BUILD_DIR)/logic.so

.PHONY: all setup clean module
