#ifndef PATH_H
#define PATH_H

#include <z_string.h>
#include <z_allocator.h>

Z_String z_expand_tilde(Z_Allocator *allocator, Z_String_View pathname);
Z_String z_compress_tilde(Z_Allocator *allocator, Z_String_View pathname);

#endif