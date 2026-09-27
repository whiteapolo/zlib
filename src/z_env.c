#include <stdlib.h>
#include "../include/z_env.h"

const char *z_env_get(const char *name, const char *fallback)
{
    const char *value = getenv(name);
    return value ? value : fallback;
}
