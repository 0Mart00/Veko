CC = gcc
CFLAGS = -Wall -Wextra -fPIC
LDFLAGS = -ldl

all: host module

host:
	$(CC) core/main.c -o build/engine_host $(LDFLAGS)

module:
	$(CC) $(CFLAGS) -shared modules/logic.c -o build/logic.so

clean:
	rm -rf build/*