#version 410

#define M_PI 3.1415926535897932384626433832795

layout(std140) uniform fw64LightingData {
    vec4 fw64_ambient_light_color;
    vec4 fw64_light_color;
    vec3 fw64_light_direction;
    float fw64_align;
};

layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
};

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_tex_coord;

out vec4 calculated_color;

void main() {
    calculated_color = fw64_ambient_light_color;

    gl_Position = fw64_mvp_matrix * vertex_position;
}