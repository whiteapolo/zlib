#ifndef Z_PATH_H
#define Z_PATH_H

#include "string.h"
#include "pool.h"

bool z_path_expand_tilde(Z_String_View path, Z_String *out);
bool z_path_compress_tilde(Z_String_View path, Z_String *out);
bool z_path_is_directory(const char *path);
bool z_path_is_regular_file(const char *path);

#endif
