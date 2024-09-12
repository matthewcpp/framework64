#pragma once

#include <stdarg.h>

/** \file log.h */

#ifdef NDEBUG
    #define fw64_debug_log(message, ...) 
#else

#ifdef __cplusplus
extern "C" {
#endif

    void fw64_debug_log(const char* message, ...);

#ifdef __cplusplus
}
#endif

#endif
