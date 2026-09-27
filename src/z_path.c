#include <z_path.h>
#include <z_env.h>
#include <sys/stat.h>

bool z_path_expand_tilde(Z_String_View pathname, Z_String *out)
{
    if (!z_sv_starts_with(pathname, z_sv("~"))) {
        z_str_append_sv(out, pathname);
        return false;
    }

    z_str_append_cstr(out, z_env_get("HOME", "."));
    z_str_append_sv(out, z_sv_advance(pathname, 1));
    return true;
}

bool z_path_compress_tilde(Z_String_View pathname, Z_String *out)
{
    const char *home = z_env_get("HOME", ".");

    if (!z_sv_starts_with(pathname, z_sv(home))) {
        z_str_append_sv(out, pathname);
        return false;
    }

    z_str_append_cstr(out, "~");
    z_str_append_sv(out, z_sv_advance(pathname, strlen(home)));
    return true;
}

bool z_path_is_directory(const char *pathname)
{
    struct stat sb;

    if (stat(pathname, &sb) == 0) {
        return S_ISDIR(sb.st_mode);
    }

    return false;
}

bool z_path_is_regular_file(const char *pathname)
{
    struct stat sb;

    if (stat(pathname, &sb) == 0) {
        return S_ISREG(sb.st_mode);
    }

    return false;
}
