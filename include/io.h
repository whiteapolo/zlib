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

#define Z_TERM_RESET             "\033[0m"
#define Z_TERM_BLACK             "\033[30m"
#define Z_TERM_RED               "\033[31m"
#define Z_TERM_GREEN             "\033[32m"
#define Z_TERM_YELLOW            "\033[33m"
#define Z_TERM_BLUE              "\033[34m"
#define Z_TERM_MAGENTA           "\033[35m"
#define Z_TERM_CYAN              "\033[36m"
#define Z_TERM_WHITE             "\033[37m"

#define Z_TERM_BRIGHT_BLACK      "\033[90m"
#define Z_TERM_BRIGHT_RED        "\033[91m"
#define Z_TERM_BRIGHT_GREEN      "\033[92m"
#define Z_TERM_BRIGHT_YELLOW     "\033[93m"
#define Z_TERM_BRIGHT_BLUE       "\033[94m"
#define Z_TERM_BRIGHT_MAGENTA    "\033[95m"
#define Z_TERM_BRIGHT_CYAN       "\033[96m"
#define Z_TERM_BRIGHT_WHITE      "\033[97m"

#define Z_TERM_BG_BLACK          "\033[40m"
#define Z_TERM_BG_RED            "\033[41m"
#define Z_TERM_BG_GREEN          "\033[42m"
#define Z_TERM_BG_YELLOW         "\033[43m"
#define Z_TERM_BG_BLUE           "\033[44m"
#define Z_TERM_BG_MAGENTA        "\033[45m"
#define Z_TERM_BG_CYAN           "\033[46m"
#define Z_TERM_BG_WHITE          "\033[47m"

#define Z_TERM_BG_BRIGHT_BLACK   "\033[100m"
#define Z_TERM_BG_BRIGHT_RED     "\033[101m"
#define Z_TERM_BG_BRIGHT_GREEN   "\033[102m"
#define Z_TERM_BG_BRIGHT_YELLOW  "\033[103m"
#define Z_TERM_BG_BRIGHT_BLUE    "\033[104m"
#define Z_TERM_BG_BRIGHT_MAGENTA "\033[105m"
#define Z_TERM_BG_BRIGHT_CYAN    "\033[106m"
#define Z_TERM_BG_BRIGHT_WHITE   "\033[107m"

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
