#pragma once

#ifdef __linux__
#include <GL/glew.h>
#else
#include <gl/glew.h>
#endif

namespace framework64 {

GLint getNextUniformBindingIndex();
void returnUniformBindingIndex(GLint index);

template <typename T>
struct UniformBlock {
    UniformBlock() = default;
    ~UniformBlock();
    T data;
    GLuint buffer = 0;
    GLuint binding_index = 0;

    void create();
    void update();
    void freeGlResources();
};

template<typename T>
UniformBlock<T>::~UniformBlock() {
    freeGlResources();
}

template<typename T>
void UniformBlock<T>::freeGlResources() {
    if (buffer > 0) {
        returnUniformBindingIndex(binding_index);
        glDeleteBuffers(1, &buffer);
        buffer = 0;
    }
}

template<typename T>
void UniformBlock<T>::create() {
    binding_index = getNextUniformBindingIndex();
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
