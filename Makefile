exe: main.c zlib.c zlib.h
	cc main.c zlib.c -o exe -g -Werror -Wextra -O3
