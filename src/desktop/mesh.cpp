#include "framework64/desktop/mesh.h"
#include "framework64/desktop/texture.h"

#include <gl/glew.h>


fw64Mesh::fw64Mesh() {
    box_invalidate(&bounding_box);
}

// C interface

void fw64_mesh_get_bounding_box(fw64Mesh* mesh, Box* box) {
    *box = mesh->bounding_box;
}