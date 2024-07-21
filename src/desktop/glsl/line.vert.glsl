layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
};

layout(location = 0) in vec4 vertex_position;

#ifdef FW64_VERTEX_COLORS
layout(location = 3) in vec4 fw64_vertex_color;
#endif

uniform vec4 diffuse_color;

out vec4 line_color;

void main() {
    line_color = diffuse_color;

#ifdef FW64_VERTEX_COLORS
    line_color *= fw64_vertex_color;
#endif

    gl_Position = fw64_mvp_matrix * vertex_position;
}
