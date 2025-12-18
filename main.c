#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wh/messure_time.h>
#include "zlib.h"

int main(void) {

    Z_Heap_Auto heap = {0};
    Z_String_Array files = z_read_directory(&heap, "../../");

    for (size_t i = 0; i < files.length; i++) {
      printf("%s\n", files.ptr[i].ptr);
    }
}
