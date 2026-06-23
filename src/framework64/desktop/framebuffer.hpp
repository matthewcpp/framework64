#pragma once

#if defined(__linux__) || defined(FW64_PLATFORM_WEB)
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

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