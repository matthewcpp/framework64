layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
};

layout(location = 0) in vec4 vertex_position;

void main() {
    gl_Position = fw64_mvp_matrix * vertex_position;
}
