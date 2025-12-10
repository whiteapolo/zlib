#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <wh/messure_time.h>
#include "zlib.h"

int main(void) {
    Z_Heap_Auto heap = {0};
    Z_Map *map = z_map_new(&heap, (Z_Compare_Fn)strcmp, NULL, NULL);
    z_map_put(map, "hey", "bye");
    printf("%s\n", (char*)z_map_get(map, "hey"));
}
