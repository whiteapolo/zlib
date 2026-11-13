#include "../include/io.h"
#include <stdlib.h>
#include <sys/stat.h>

#define PIPE_IN 1
#define PIPE_OUT 0

void z_safe_pipe(int fd[2]);
int z_safe_fork(void);

long z_get_file_size(FILE *fp)
{
    long curr = ftell(fp);
    assert(curr >= 0);
    fseek(fp, 0, SEEK_END);

    long size = ftell(fp);
    assert(size >= 0);
    fseek(fp, curr, SEEK_SET);

    return size;
}

bool z_file_read(const char *path, Z_String *s)
{
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        return false;
    }

    size_t file_size = (size_t)z_get_file_size(fp);

    z_array_reserve(s, s->length + file_size);
    s->length += fread(s->ptr + s->length, 1, file_size, fp);
    z_array_zero_terminate(s);
    fclose(fp);

    return true;
}


bool z_file_write(const char *path, const char *format, ...)
{
    FILE *fp = fopen(path, "w");

    if (fp == NULL) {
        return false;
    }

    va_list ap;
    va_start(ap, format);
    vfprintf(fp, format, ap);
    va_end(ap);
    fclose(fp);

    return true;
}

bool z_file_append(const char *path, const char *format, ...)
{
    FILE *fp = fopen(path, "a");

    if (fp == NULL) {
        return false;
    }

    va_list ap;
    va_start(ap, format);
    vfprintf(fp, format, ap);
    va_end(ap);
    fclose(fp);

    return true;
}

bool z_file_scanf(const char *path, const char *format, ...)
{
    FILE *fp = fopen(path, "r");

    if (fp == NULL) {
        return false;
    }

    va_list ap;
    va_start(ap, format);

    if (vfscanf(fp, format, ap) == EOF) {
        va_end(ap);
        fclose(fp);
        return false;
    }

    va_end(ap);
    fclose(fp);

    return true;
}

size_t z_file_read_line(FILE *fp, Z_String *out)
{
    char buffer[READ_BUFFER_SIZE] = {0};
    z_str_clear(out);

    while (fgets(buffer, READ_BUFFER_SIZE, fp)) {
        z_str_push_cstr(out, buffer);

        if (strchr(buffer, '\n')) {
            break;
        }
    }

    return out->length;
}

void z_safe_pipe(int fd[2])
{
    if (pipe(fd) == -1) {
        z_perror_die("pipe() failed");
    }
}

int z_safe_fork(void)
{
    int pid = fork();

    if (pid == -1) {
        z_perror_die("fork() failed");
    }

    return pid;
}

Z_Piped_Process z_pipe_process(char *args[], Z_File_Redirect redirect)
{
    int stdin[2];
    int stdout[2];
    int stderr[2];

    if (redirect & Z_File_Redirect_Stdin) z_safe_pipe(stdin);
    if (redirect & Z_File_Redirect_Stdout) z_safe_pipe(stdout);
    if (redirect & Z_File_Redirect_Stderr) z_safe_pipe(stderr);

    int pid = z_safe_fork();

    if (pid == 0) { // child
        if (redirect & Z_File_Redirect_Stdin) {
            dup2(stdin[PIPE_OUT], STDIN_FILENO);
            close(stdin[PIPE_IN]);
        }

        if (redirect & Z_File_Redirect_Stdout) {
            dup2(stdout[PIPE_IN], STDOUT_FILENO);
            close(stdout[PIPE_OUT]);
        }

        if (redirect & Z_File_Redirect_Stderr) {
            dup2(stderr[PIPE_IN], STDERR_FILENO);
            close(stderr[PIPE_OUT]);
        }

        execvp(args[0], args);
        perror("exec() failed");
        exit(EXIT_FAILURE);
    }

    Z_Piped_Process piped_process = {0};

    if (redirect & Z_File_Redirect_Stdin) {
        close(stdin[PIPE_OUT]);
        piped_process.stdin = fdopen(stdin[PIPE_IN], "w");
    }

    if (redirect & Z_File_Redirect_Stdout) {
        close(stdout[PIPE_IN]);
        piped_process.stdout = fdopen(stdout[PIPE_OUT], "r");
    }

    if (redirect & Z_File_Redirect_Stderr) {
        close(stderr[PIPE_IN]);
        piped_process.stderr = fdopen(stderr[PIPE_OUT], "r");
    }

    return piped_process;
}

bool z_expand_tilde(Z_String_View path, Z_String *out)
{
    if (!z_sv_starts_with(path, z_sv("~"))) {
        z_str_push_sv(out, path);
        return false;
    }

    z_str_push_cstr(out, z_env("HOME", "."));
    z_str_push_sv(out, z_sv_advance(path, 1));
    return true;
}

bool z_compress_tilde(Z_String_View path, Z_String *out)
{
    const char *home = z_env("HOME", ".");

    if (!z_sv_starts_with(path, z_sv(home))) {
        z_str_push_sv(out, path);
        return false;
    }

    z_str_push_cstr(out, "~");
    z_str_push_sv(out, z_sv_advance(path, strlen(home)));
    return true;
}

bool z_is_directory(const char *path)
{
    struct stat sb;

    if (stat(path, &sb) == 0) {
        return S_ISDIR(sb.st_mode);
    }

    return false;
}

bool z_is_regular_file(const char *path)
{
    struct stat sb;

    if (stat(path, &sb) == 0) {
        return S_ISREG(sb.st_mode);
    }

    return false;
}

const char *z_env(const char *name, const char *fallback)
{
    const char *value = getenv(name);
    return value ? value : fallback;
}
