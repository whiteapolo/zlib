#include <stdio.h>
#include <assert.h>
#include "zlib.h"


int main(void)
{
  Z_Char *content = z_read_file("./zlib.c");
  assert(content != NULL);

  Z_Char **lines = z_str_split(z_sv(content), z_sv(";"));

  for (size_t i = 0; i < z_array_length(lines); i++) {
    printf("%zu | %s\n", i, lines[i]);
  }

  z_str_array_free(&lines);
  z_str_free(&content);
}
