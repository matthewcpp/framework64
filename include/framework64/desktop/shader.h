#pragma once

#include <string>

namespace framework64::Shader {
    uint32_t createFromPaths(std::string const & vertex_path, std::string const & frag_path);
    uint32_t compile(std::string const& vertex, std::string const& fragment);
}