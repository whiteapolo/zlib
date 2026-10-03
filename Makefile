CC := gcc
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

libzatar:
	@mkdir -p libzatar
	@$(CC) $(CFLAGS) -c src/all.c -o libzatar/libzatar.o
	@ar rcs libzatar/libzatar.a libzatar/libzatar.o
	@$(CC) libzatar/obj/libzatar.o -shared -o libzatar/libzatar.so

release:
	@make libzatar

debug:
	@make libzatar CFLAGS=-g -O0

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
