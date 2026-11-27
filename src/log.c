#include "../include/log.h"
#include "../include/io.h"
#include <stdio.h>
#include <unistd.h>

static Z_Log_Level z__current_log_level = Z_LOG_INFO;

static const char *z__log_level_to_string(Z_Log_Level level)
{
    switch (level) {
        case Z_LOG_DEBUG: return "DEBUG";
        case Z_LOG_INFO:  return "INFO";
        case Z_LOG_WARN:  return "WARN";
        case Z_LOG_ERROR: return "ERROR";
        case Z_LOG_FATAL: return "FATAL";
        case Z_LOG_NONE:  return "NONE";
        default:          return "UNKNOWN";
    }
}

void z_set_log_level(Z_Log_Level level)
{
    z__current_log_level = level;
}

static bool z__does_support_color()
{
    static bool first = true;
    static bool cache = false;

    if (first) {
        cache = isatty(STDERR_FILENO);
    }

    return cache;
}

static const char *z__get_log_level_label_color(Z_Log_Level level)
{
    switch (level) {
        case Z_LOG_DEBUG: return Z_TERM_MAGENTA;
        case Z_LOG_INFO:  return Z_TERM_GREEN;
        case Z_LOG_WARN:  return Z_TERM_YELLOW;
        case Z_LOG_ERROR: return Z_TERM_RED;
        case Z_LOG_FATAL: return Z_TERM_RED;
        case Z_LOG_NONE:  return "";
        default:          return "";
    }
}

static void z__print_log_label(Z_Log_Level level)
{
    if (z__does_support_color()) {
        const char *color = z__get_log_level_label_color(level);
        const char *label = z__log_level_to_string(level);
        fprintf(stderr, "[%s%s%s] ", color, label, Z_TERM_RESET);
    } else {
        fprintf(stderr, "[%s] ", z__log_level_to_string(level));
    }
}

static void z__log(Z_Log_Level level, const char *format, va_list ap)
{
    if (z__current_log_level > level) {
        return;
    }

    z__print_log_label(level);
    vfprintf(stderr, format, ap);
    fputc('\n', stderr);
}

void z_log_debug(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z__log(Z_LOG_DEBUG, format, ap);
    va_end(ap);
}

void z_log_info(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z__log(Z_LOG_INFO, format, ap);
    va_end(ap);
}

void z_log_warn(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z__log(Z_LOG_WARN, format, ap);
    va_end(ap);
}

void z_log_error(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z__log(Z_LOG_ERROR, format, ap);
    va_end(ap);
}

void z_log_fatal(const char *format, ...)
{
    va_list ap;
    va_start(ap, format);
    z__log(Z_LOG_FATAL, format, ap);
    va_end(ap);
}
