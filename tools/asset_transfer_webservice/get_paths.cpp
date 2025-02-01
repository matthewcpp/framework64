#include "get_paths.hpp"

// https://stackoverflow.com/questions/1528298/get-path-of-executable
// https://stackoverflow.com/questions/22675457/what-is-the-equivalent-of-proc-self-exe-on-macintosh-os-x-mavericks

#include <filesystem>
#include <stdlib.h>

#ifdef __APPLE__

#include <mach-o/dyld.h>

static std::string get_executable_path_impl() {
    char* exe_path_buffer = NULL;
    uint32_t buff_size = 0;

    // this will give us the size of the buffer we need to hold the path
    _NSGetExecutablePath(NULL, &buff_size);
    buff_size += 1;

    exe_path_buffer = reinterpret_cast<char*>(alloca(buff_size));
    _NSGetExecutablePath(exe_path_buffer, &buff_size);
    
    // Path returned above may have symlinks or '..' etc... use realpath to cannoticalize it
    char path_buffer [PATH_MAX+1];
    const char* real_path = realpath(exe_path_buffer, path_buffer);

    return std::string(real_path);
}

#endif

namespace framework64::service {

const std::string& get_executable_path() {
    static std::string executable_path = get_executable_path_impl();

    return executable_path;
}

const std::string get_executable_dir() {
    static std::string executable_dir = std::filesystem::path(get_executable_path()).parent_path().string();

    return executable_dir;
}

const std::string get_framework64_dir() {
    const auto framework64_dir_path = std::filesystem::canonical(std::filesystem::path(get_executable_path()).parent_path() / ".." / ".." / "..");

    if (!std::filesystem::exists(framework64_dir_path)) {
        return {};
    }

    //validate that the directory contains the pipeline
    const auto pipeline_file_path = framework64_dir_path /  "pipeline" / "pipeline.js";

    if (!std::filesystem::exists(pipeline_file_path)) {
        return {};
    }

    return framework64_dir_path.string();
}
// "/Users/mlarocca/development/repos/framework64/build_desktop/tools/asset_transfer_webservice"

}