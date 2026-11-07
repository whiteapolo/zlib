#include "../include/path.h"
#include "../include/env.h"
#include <sys/stat.h>

bool z_path_expand_tilde(Z_String_View path, Z_String *out)
{
    if (!z_sv_starts_with(path, z_sv("~"))) {
        z_str_push_sv(out, path);
        return false;
    }

    z_str_push_cstr(out, z_env_get("HOME", "."));
    z_str_push_sv(out, z_sv_advance(path, 1));
    return true;
}

bool z_path_compress_tilde(Z_String_View path, Z_String *out)
{
    const char *home = z_env_get("HOME", ".");

    if (!z_sv_starts_with(path, z_sv(home))) {
        z_str_push_sv(out, path);
        return false;
    }

    z_str_push_cstr(out, "~");
    z_str_push_sv(out, z_sv_advance(path, strlen(home)));
    return true;
}

bool z_path_is_directory(const char *path)
{
    struct stat sb;

    if (stat(path, &sb) == 0) {
        return S_ISDIR(sb.st_mode);
    }

    return false;
}

bool z_path_is_regular_file(const char *path)
{
    struct stat sb;

    if (stat(path, &sb) == 0) {
        return S_ISREG(sb.st_mode);
    }

    return false;
}
