#pragma once

#include "framework64/modules.h"

#ifndef FW64_STATIC_MODULE_COUNT
#define FW64_STATIC_MODULE_COUNT 2
#endif

typedef struct {
    fw64StaticModuleId id;
    void* module;
    fw64ModuleUpdateFunc update_func;
    void* update_arg;
} fw64StaticModule;

struct fw64Modules {
    fw64StaticModule static_modules[FW64_STATIC_MODULE_COUNT];
    uint32_t static_module_count;
};

void fw64_n64_libultra_modules_init(fw64Modules* modules);
void fw64_n64_libultra_modules_update(fw64Modules* modules);
