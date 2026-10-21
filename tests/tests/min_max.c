#include <stdio.h>
#include "../all.h"
void main() { int a=3,b=8; printf("min=%d max=%d nested=%d\n",Z_MIN(a,b),Z_MAX(a,b),Z_MAX(Z_MIN(9,2),Z_MIN(5,7))); }
