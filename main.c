#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wh/messure_time.h>
#include "zlib.h"

int main(void)
{
    Z_Heap_Auto heap = {0};
    Z_Maybe_String_Array result = z_read_directory(&heap, "../../fff");

    if (!result.ok) {
      return -1;
    }

    Z_String_Array files = result.value;

    for (size_t i = 0; i < files.length; i++) {
      if (!z_sv_starts_with(z_sv_from_str(&files.ptr[i]), z_sv_from_cstr("."))) {
        printf("%s\n", files.ptr[i].ptr);
      }
    }
}
