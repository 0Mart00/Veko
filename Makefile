CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -ldl
BUILD_DIR = build

all: setup host module

setup:
	mkdir -p $(BUILD_DIR)

host: setup
	$(CC) core/main.c -o $(BUILD_DIR)/engine_host $(LDFLAGS)

module: setup
	$(CC) $(CFLAGS) -shared modules/logic.c -o $(BUILD_DIR)/logic.so

clean:
	rm -rf $(BUILD_DIR)/*