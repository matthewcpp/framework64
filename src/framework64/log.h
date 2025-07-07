#pragma once

#include <stdarg.h>

/** \file log.h */

#ifdef NDEBUG
    #define fw64_log_message(message, ...) 
#else

#ifdef __cplusplus
extern "C" {
#endif

    void fw64_log_message(const char* message, ...);

#ifdef __cplusplus
}
#endif

#endif
