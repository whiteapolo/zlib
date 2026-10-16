#include <stdio.h>
#include <stdlib.h>
#include "../all.h"
void main() {
    setenv("ZLIB_TEST_SET", "set-value", 1);
    printf("fallback=%s\n", z_env_get("ZLIB_TEST_VAR_UNSET_8472", "fallback"));
    printf("set=%s\n", z_env_get("ZLIB_TEST_SET", "missing"));
}
