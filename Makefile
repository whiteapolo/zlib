MAKEFLAGS += --no-print-directory
CC := cc
BASE_CFLAGS :=            \
	-O3                   \
	-fPIC                 \
    -Wall                 \
    -Wextra               \
    -Werror               \
    -Wconversion          \
    -Wsign-conversion     \
    -Wformat=2            \
    -Wcast-qual           \
    -Wswitch-enum         \
    -Wmissing-prototypes

all: release

release:
	@make build CFLAGS=-O3

debug:
	@make build 'CFLAGS=-g -O0'

build:
	@mkdir -p build
	@$(CC) $(BASE_CFLAGS) $(CFLAGS) -c src/all.c -o build/libzatar.o
	@ar rcs build/libzatar.a build/libzatar.o
	@$(CC) build/libzatar.o -shared -o build/libzatar.so

install:
	@mkdir -p /usr/local/include/libzatar
	@cp -r include/* /usr/local/include/libzatar/
	@mkdir -p /usr/local/lib
	@cp build/libzatar.a build/libzatar.so /usr/local/lib

uninstall:
	@rm -rf                               \
		/usr/local/include/libzatar       \
		/usr/local/lib/libzatar.so        \
		/usr/local/lib/libzatar.a

clean:
	@rm -rf build

.PHONY: all build install uninstall clean
