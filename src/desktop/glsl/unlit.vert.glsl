layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
};

layout(location = 0) in vec4 fw64_vertex_position;

#ifdef FW64_VERTEX_COLORS
layout(location = 3) in vec4 fw64_vertex_color;
#endif

out vec4 unlit_color;

void main() {
    gl_Position = fw64_mvp_matrix * fw64_vertex_position;

    #ifdef FW64_VERTEX_COLORS
    unlit_color = fw64_vertex_color;
    #endif
}
