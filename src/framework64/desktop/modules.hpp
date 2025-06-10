#include "framework64/modules.h"

#include <vector>

struct fw64Modules {
    struct StaticRecord {
        StaticRecord(fw64StaticModuleId id, void* module, fw64ModuleUpdateFunc update_func, void* update_arg): 
            id(id), module(module), update_func(update_func), update_arg(update_arg)
        {}

        fw64StaticModuleId id;
        void* module;
        fw64ModuleUpdateFunc update_func;
        void* update_arg;
    };

    std::vector<StaticRecord> static_modules;

    void update();
};
