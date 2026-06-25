#pragma once

#include "framework64/desktop/openGL.hpp"

#include <memory>
#include <string>
#include <vector>

namespace framework64 {

struct Framebuffer {
public:
    bool initialize(GLuint buffer_width, GLuint buffer_height);

public:
    GLuint width, height;
    GLuint framebuffer_handle;
    GLuint color_attachment;
    GLuint depth_attachment;
};

}