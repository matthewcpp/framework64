#include "modules.h"

#include <stdlib.h>

void fw64_n64_libultra_modules_init(fw64Modules* modules) {
    modules->static_module_count = 0;
}

void fw64_n64_libultra_modules_update(fw64Modules* modules) {
    for (uint32_t i = 0; i < modules->static_module_count; i++) {
        fw64StaticModule* static_module = &modules->static_modules[i];
        if (static_module->update_func) {
            static_module->update_func(static_module->module, static_module->update_arg);
        }
    }
}

int _fw64_modules_register_static(fw64Modules* modules, fw64StaticModuleId id, void* module, fw64ModuleUpdateFunc update_func, void* update_arg) {
    if (modules->static_module_count >= FW64_STATIC_MODULE_COUNT || fw64_modules_get_static(modules, id)) {
        return 0;
    }

    fw64StaticModule* static_module = &modules->static_modules[modules->static_module_count++];
    static_module->id = id;
    static_module->module = module;
    static_module->update_func = update_func;
    static_module->update_arg = update_arg;

    return 1;
}

void* fw64_modules_get_static(fw64Modules* modules, fw64StaticModuleId id) {
    for (uint32_t i = 0; i < modules->static_module_count; i++) {
        if (modules->static_modules[i].id == id) {
            return &modules->static_modules[i];
        }
    }

    return NULL;
}