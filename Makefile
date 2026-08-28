CC = gcc
BASE_CFLAGS = \
    -I./include \
    -std=c2x \
    -Wall \
    -Wextra \
    -Wpedantic \
    -Werror \
    -Wshadow \
    -Wconversion \
    -Wsign-conversion \
    -Wformat=2 \
    -Wnull-dereference \
    -Wundef \
    -Wcast-qual \
    -Wvla \
    -Wswitch-enum \
    -Wmissing-prototypes

PROD_CFLAGS = $(BASE_CFLAGS) $(WARN_CFLAGS) -O3
DEV_CFLAGS = $(BASE_CFLAGS) $(WARN_CFLAGS) -O0 -g

all: release

release:
	@mkdir -p obj
	$(CC) $(PROD_CFLAGS) -c ./src/all.c -o obj/libzatar.o
	ar rcs libzatar.a ./obj/libzatar.o

dev:
	@mkdir -p obj
	$(CC) $(DEV_CFLAGS) -c ./src/all.c -o obj/libzatar.o
	ar rcs libzatar.a ./obj/libzatar.o

clean:
	rm -rf obj libzatar.a

.PHONY: all clean release dev
