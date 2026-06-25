#pragma once

#include <stdarg.h>

/** \file log.h */

#ifdef __cplusplus
extern "C" {
#endif

    void fw64_log_message(const char* message, ...);

#ifdef __cplusplus
}
#endif
