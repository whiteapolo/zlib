#include <stdio.h>

void z_term_disable_line_wrap(void)
{
    printf("\033[?7l");
}

void z_term_enable_line_wrap(void)
{
    printf("\033[?7h");
}

void z_term_hide_cursor(void)
{
    printf("\033[?25l");
}

void z_term_show_cursor(void)
{
    printf("\033[?25h");
}

void z_term_set_cursor_position(int x, int y)
{
    printf("\033[%d;%dH", y, x);
}

void z_term_set_cursor_horizontal_position(int x)
{
    printf("\033[%dG", x);
}

void z_term_move_cursor_up(int n)
{
    printf("\033[%dA", n);
}

void z_term_move_cursor_down(int n)
{
    printf("\033[%dB", n);
}

void z_term_move_cursor_right(int n)
{
    printf("\033[%dC", n);
}

void z_term_move_cursor_left(int n)
{
    printf("\033[%dD", n);
}

void z_term_enter_alternative_screen(void)
{
    printf("\033[?1049h");
}

void z_term_exit_alternative_screen(void)
{
    printf("\033[?1049l");
}

void z_term_clear_line(void)
{
    printf("\033[K");
}

void z_term_clear_screen(void)
{
    printf("\033[2J");
}
