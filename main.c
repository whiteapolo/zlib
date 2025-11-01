#include <stdio.h>
#include "zlib.h"
#include <string.h>

void print_string(char *s)
{
  printf("%s\n", s);
}

int cmp_strings_ptr(const char **a, const char **b)
{
    return strcmp(*a, *b);
}

int main(void)
{
  Z_Char **entries = z_read_directory(".");
  z_array_sort(&entries, (Z_Compare_Fn)cmp_strings_ptr);
  z_array_foreach(entries, print_string);
  z_str_array_free(&entries);
}
