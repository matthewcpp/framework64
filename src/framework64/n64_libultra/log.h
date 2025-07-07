#pragma once

#include "framework64/log.h"

#include <stdio.h>

typedef void (*fw64N64LibultraLogFunc) (const char* message, va_list);

#ifndef NDEBUG
void fw64_n64libultra_log_set_func(fw64N64LibultraLogFunc func);
#else
#define fw64_n64libultra_log_set_func(func)
#endif
