exe: main.c zlib.c zlib.h
	cc main.c zlib.c -o exe -g -Wall -Werror -Wextra -O3

run: exe
	./exe