layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
    float fw64_camera_near;
    float fw64_camera_far;
};

layout(location = 0) in vec4 fw64_vertex_position;
layout(location = 2) in vec2 fw64_vertex_tex_coord;
layout(location = 3) in vec4 fw64_vertex_color;

out vec2 tex_coords;
out vec4 vertex_color;

void main() {
    tex_coords = fw64_vertex_tex_coord;
    vertex_color = fw64_vertex_color;
    gl_Position = fw64_mvp_matrix * fw64_vertex_position;
}
