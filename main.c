#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wh/messure_time.h>
#include "zlib.h"

int main(void) {
    Z_Clock start = z_get_clock();
    Z_Heap_Auto heap = {0};
    Z_Char **files = z_read_directory(&heap, "../../");
    
    z_array_sort(&files, (Z_Compare_Fn)z_compare_string_pointers);

     z_print_elapsed_seconds(start);
    for (size_t i = 0; i < z_array_length(files); i++) {
      printf("%s,", files[i]);
    }

   
}
