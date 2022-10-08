layout(std140) uniform fw64SpriteTransformData {
    mat4 fw64_mvp_matrix;
};

layout(location = 0) in vec4 fw64_vertex_position;
layout(location = 3) in vec4 fw64_vertex_color;

out vec4 vertex_color;

void main() {
    vertex_color = fw64_vertex_color;
    gl_Position = fw64_mvp_matrix * fw64_vertex_position;
}
