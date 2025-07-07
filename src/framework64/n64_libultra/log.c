#include "framework64/n64_libultra/log.h"

#include <stdlib.h>

static fw64N64LibultraLogFunc log_func = NULL;

#ifndef NDEBUG

void fw64_n64libultra_log_set_func(fw64N64LibultraLogFunc func) {
    log_func = func;
}

void fw64_log_message(const char* message, ...) {
    if (log_func) {
        va_list args;
        va_start (args, message);
        log_func(message, args);
        va_end(args);
    }
}

#endif

