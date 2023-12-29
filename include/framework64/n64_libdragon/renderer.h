#pragma once

#include "framework64/renderer.h"

#include <GL/gl.h>


struct fw64Renderer {
    int temp;
};

void fw64_renderer_draw_unlit_vertex_color_mesh(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh);
void fw64_renderer_draw_lit_mesh_textured(fw64Renderer* renderer, fw64Transform* transform, fw64Mesh* mesh);

