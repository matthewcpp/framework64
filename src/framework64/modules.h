#pragma once

/** \file modules.h */

#include <stdint.h>

typedef uint64_t fw64StaticModuleId;

typedef struct fw64Modules fw64Modules;

typedef void(*fw64ModuleUpdateFunc)(void* module, void* arg);

#ifdef __cplusplus
extern "C" {
#endif

/** Registers a static module with the system. 
 * This private method should be invoked during module initialization and should not be direclty called from application logic. 
 * Returns non zero value if the module was sucessfully registered. */
int _fw64_modules_register_static(fw64Modules* modules, fw64StaticModuleId id, void* module, fw64ModuleUpdateFunc update_func, void* update_arg);

/** Retrieves a registered static module with the given Id.  Returns NULL if the module has not been registered. */
void* fw64_modules_get_static(fw64Modules* modules, fw64StaticModuleId id);

#ifdef __cplusplus
}
#endif
