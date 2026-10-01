CC = gcc
BASE_CFLAGS =             \
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

RELEASE_CFLAGS = $(BASE_CFLAGS) -O3
DEV_CFLAGS = $(BASE_CFLAGS) -O0 -g

all: release

release:
	@mkdir -p libzatar
	@$(CC) $(RELEASE_CFLAGS) -c src/all.c -o libzatar/libzatar.o
	@ar rcs libzatar/libzatar.a libzatar/libzatar.o
	@$(CC) libzatar/libzatar.o -shared -o libzatar/libzatar.so

dev:
	@mkdir -p libzatar
	@$(CC) $(DEV_CFLAGS) -c src/all.c -o libzatar/libzatar.o
	@ar rcs libzatar/libzatar.a libzatar/libzatar.o
	@$(CC) $(DEV_CFLAGS) libzatar/libzatar.o -shared -o libzatar/libzatar.so

install: install-include install-binary

install-include:
	@mkdir -p /usr/local/include/libzatar
	@cp -r include/* /usr/local/include/libzatar/

install-binary:
	@mkdir -p /usr/local/lib
	@cp libzatar/libzatar.so libzatar/libzatar.a /usr/local/lib/

uninstall: uninstall-include uninstall-binary

uninstall-include:
	@rm -rf /usr/local/include/libzatar

uninstall-binary:
	@rm -f /usr/local/lib/libzatar.so /usr/local/lib/libzatar.a

clean:
	@rm -rf libzatar

.PHONY: all release dev install install-include install-binary uninstall uninstall-include uninstall-binary clean
