#include "framework64/log.h"

#include <cstdarg>
#include <cstdio>

void fw64_log_message(const char* message, ...) {
    va_list args;
    va_start(args, message);
    vprintf(message, args);
    va_end(args);
    puts("");
}