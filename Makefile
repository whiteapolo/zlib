CC := gcc
BASE_CFLAGS :=            \
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

RELEASE_CFLAGS := $(BASE_CFLAGS) -O3
DEBUG_CFLAGS := $(BASE_CFLAGS) -O0 -g

all: release

libzatar:
	@mkdir -p libzatar/obj
	@$(CC) $(CFLAGS) -c src/all.c -o libzatar/libzatar.o
	@ar rcs libzatar/libzatar.a libzatar/obj/libzatar.o
	@$(CC) libzatar/obj/libzatar.o -shared -o libzatar/libzatar.so

install: libzatar
	@mkdir -p /usr/local/include/libzatar
	@cp -r include/* /usr/local/include/libzatar/
	@mkdir -p /usr/local/lib

uninstall:
	@rm -rf                               \
		/usr/local/include/libzatar       \
		/usr/local/lib/libzatar.so        \
		/usr/local/lib/libzatar.a

clean:
	@rm -rf obj libzatar

.PHONY: all build install uninstall clean
