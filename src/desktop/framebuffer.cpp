#include "framework64/desktop/framebuffer.hpp"



#include <algorithm>

namespace framework64 {

bool Framebuffer::initialize(GLuint buffer_width, GLuint buffer_height) {
    width = buffer_width;
    height = buffer_height;

    glGenFramebuffers(1, &framebuffer_handle);
    glGenTextures(1, &color_attachment);

    glBindTexture(GL_TEXTURE_2D, color_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_RGB, width, height, 0, GL_RGB, GL_UNSIGNED_BYTE, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_LINEAR);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);

    glGenTextures(1, &depth_attachment);
    glBindTexture(GL_TEXTURE_2D, depth_attachment);
    glTexImage2D(GL_TEXTURE_2D, 0, GL_DEPTH_COMPONENT24, width, height, 0, GL_DEPTH_COMPONENT, GL_UNSIGNED_INT, nullptr);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
    glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);

    glBindTexture(GL_TEXTURE_2D, 0);

    glBindFramebuffer(GL_FRAMEBUFFER, framebuffer_handle);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_COLOR_ATTACHMENT0, GL_TEXTURE_2D, color_attachment, 0);
    glFramebufferTexture2D(GL_FRAMEBUFFER, GL_DEPTH_ATTACHMENT, GL_TEXTURE_2D, depth_attachment, 0);

    auto result = glCheckFramebufferStatus(GL_FRAMEBUFFER);

    if (result != GL_FRAMEBUFFER_COMPLETE) {
        glDeleteFramebuffers(1, &framebuffer_handle);
        glDeleteTextures(1, &color_attachment);
        glDeleteTextures(1, &depth_attachment);

        return false;
    }

    glBindFramebuffer(GL_FRAMEBUFFER, 0);

    return true;
}

}