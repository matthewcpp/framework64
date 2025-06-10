#include "framework64/desktop/modules.hpp"

#include <algorithm>

int _fw64_modules_register_static(fw64Modules* modules, fw64StaticModuleId id, void* module, fw64ModuleUpdateFunc update_func, void* update_arg) {
    if (fw64_modules_get_static(modules, id)) {
        return 0;
    }

    modules->static_modules.emplace_back(id, module, update_func, update_arg);

    return 1;
}

void* fw64_modules_get_static(fw64Modules* modules, fw64StaticModuleId id) {
    auto result = std::find_if(modules->static_modules.begin(), modules->static_modules.end(), [&id](const fw64Modules::StaticRecord& record) {
        return record.id == id;
    });

    return result == modules->static_modules.end() ? nullptr : result->module;
}

void fw64Modules::update() {
    for (const auto& static_module : static_modules) {
        if (static_module.update_func) {
            static_module.update_func(static_module.module, static_module.update_arg);
        }
    }
}