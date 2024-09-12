layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
    float fw64_camera_near;
    float fw64_camera_far;
};

layout(location = 0) in vec4 fw64_vertex_position;

#ifdef FW64_DIFFUSE_TEXTURE
layout(location = 2) in vec2 fw64_vertex_tex_coord;
out vec2 tex_coords;
#endif

#ifdef FW64_VERTEX_COLORS
layout(location = 3) in vec4 fw64_vertex_color;
#endif

uniform vec4 diffuse_color;

out vec4 unlit_color;

void main() {
    gl_Position = fw64_mvp_matrix * fw64_vertex_position;

    unlit_color = diffuse_color;

    #ifdef FW64_VERTEX_COLORS
    unlit_color *= fw64_vertex_color;
    #endif

    #ifdef FW64_DIFFUSE_TEXTURE
    tex_coords = fw64_vertex_tex_coord;
    #endif
}
