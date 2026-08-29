#ifndef Z_MATH
#define Z_MATH

#include <stddef.h>
#include <sys/types.h>

#define Z_MAX(a, b)    \
({                     \
    typeof(a) _a = (a);  \
    typeof(b) _b = (b);  \
    _a > _b ? _a : _b; \
})

#define Z_MIN(a, b)    \
({                     \
    typeof(a) _a = (a);  \
    typeof(b) _b = (b);  \
    _a < _b ? _a : _b; \
})

#endif
