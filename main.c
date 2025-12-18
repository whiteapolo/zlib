#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wh/messure_time.h>
#include "zlib.h"

int main(void) {
    Z_Heap_Auto heap = {0};
    Z_Char **files = z_read_directory(&heap, "../../");

    Z_Set set = z_set_new(&heap, (Z_Compare_Fn)strcmp);

    for (size_t i = 0; i < z_array_length(files); i++) {
      z_set_add(&set, files[i]);
    }

    printf("result: %d\n", z_set_has(&set, "songs"));
}
