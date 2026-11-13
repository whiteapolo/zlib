#ifndef Z_IO_H
#define Z_IO_H

#include <stdio.h>
#include "string.h"
#include <dirent.h>

typedef enum {
    Z_File_Redirect_Stdin  = 1 << 0,
    Z_File_Redirect_Stdout = 1 << 1,
    Z_File_Redirect_Stderr = 1 << 2,
} Z_File_Redirect;

typedef struct {
    FILE *stdin;
    FILE *stdout;
    FILE *stderr;
} Z_Piped_Process;

const char *z_env(const char *name, const char *fallback);

long z_get_file_size(FILE *fp);

void z_die(const char *format, ...);
void z_die_va(const char *format, va_list ap);
void z_perror(const char *format, ...);
void z_perror_va(const char *format, va_list ap);
void z_perror_die(const char *format, ...);
void z_perror_die_va(const char *format, va_list ap);
void z_enforce(bool predicate, const char *format, ...);

bool z_file_read(const char *path, Z_String *s);
bool z_file_write(const char *path, const char *format, ...);
bool z_file_append(const char *path, const char *format, ...);
bool z_file_scanf(const char *path, const char *format, ...);
size_t z_file_read_line(FILE *fp, Z_String *out);

bool z_expand_tilde(Z_String_View path, Z_String *out);
bool z_compress_tilde(Z_String_View path, Z_String *out);
bool z_is_directory(const char *path);
bool z_is_regular_file(const char *path);

Z_Piped_Process z_pipe_process(char *args[], Z_File_Redirect redirect);

#define z_term_disable_line_wrap()               printf("\033[?7l")
#define z_term_enable_line_wrap()                printf("\033[?7h")
#define z_term_hide_cursor()                     printf("\033[?25l")
#define z_term_show_cursor()                     printf("\033[?25h")
#define z_term_set_cursor_position(x, y)         printf("\033[%d;%dH", y, x)
#define z_term_set_cursor_horizontal_position(x) printf("\033[%dG", x)
#define z_term_move_cursor_up(n)                 printf("\033[%dA", n)
#define z_term_move_cursor_down(n)               printf("\033[%dB", n)
#define z_term_move_cursor_right(n)              printf("\033[%dC", n)
#define z_term_move_cursor_left(n)               printf("\033[%dD", n)
#define z_term_enter_alternative_screen()        printf("\033[?1049h")
#define z_term_exit_alternative_screen()         printf("\033[?1049l")
#define z_term_clear_line()                      printf("\033[K")
#define z_term_clear_screen()                    printf("\033[2J")

#endif
