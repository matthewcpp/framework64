#include "framework64/desktop/uniform_block.h"

#include <vector>

namespace framework64 {

static GLint next_binding_index = 0;
static std::vector<GLint> free_binding_indices;

GLint getNextUniformBindingIndex() {
    GLint binding_index;

    if (!free_binding_indices.empty()) {
        binding_index = free_binding_indices.back();
        free_binding_indices.pop_back();
    }
    else {
        binding_index = next_binding_index++;
    }
    
    return binding_index;
}

void returnUniformBindingIndex(GLint index) {
    free_binding_indices.push_back(index);
}

}