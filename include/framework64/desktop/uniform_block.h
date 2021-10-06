#pragma once

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

namespace framework64 {

template <typename T>
struct UniformBlock {
    T data;
    GLuint buffer;
    GLuint binding_index = 0;

    void create(GLuint bind_index);
    void update();
};

template<typename T>
void UniformBlock<T>::create(GLuint bind_index) {
    binding_index = bind_index;
    glGenBuffers(1, &buffer);
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferData(GL_UNIFORM_BUFFER, sizeof(T), &data, GL_DYNAMIC_DRAW);
    glBindBufferRange(GL_UNIFORM_BUFFER, binding_index, buffer, 0, sizeof(T));
}

template<typename T>
void UniformBlock<T>::update() {
    glBindBuffer(GL_UNIFORM_BUFFER, buffer);
    glBufferSubData(GL_UNIFORM_BUFFER, 0, sizeof(T), &data);
}

}
