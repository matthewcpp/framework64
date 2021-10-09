layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
};

layout(location = 0) in vec4 fw64_vertex_position;
layout(location = 2) in vec2 fw64_vertex_tex_coord;

out vec2 tex_coords;

void main() {
    tex_coords = fw64_vertex_tex_coord;
    gl_Position = fw64_mvp_matrix * fw64_vertex_position;
}
