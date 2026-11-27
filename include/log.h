#ifndef Z_LOG_H
#define Z_LOG_H

typedef enum {
    Z_LOG_DEBUG = 0,
    Z_LOG_INFO  = 1,
    Z_LOG_WARN  = 2,
    Z_LOG_ERROR = 3,
    Z_LOG_FATAL = 4,
    Z_LOG_NONE  = 5,
} Z_Log_Level;

void z_set_log_level(Z_Log_Level level);

void z_log_debug(const char *format, ...);
void z_log_info(const char *format, ...);
void z_log_warn(const char *format, ...);
void z_log_error(const char *format, ...);
void z_log_fatal(const char *format, ...);

#endif
