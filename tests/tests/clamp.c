#include <stdio.h>
#include "../all.h"
void main() { printf("%d %d %d\n", Z_CLAMP(0, -3, 10), Z_CLAMP(0, 4, 10), Z_CLAMP(0, 13, 10)); }
