#version 410

#define M_PI 3.1415926535897932384626433832795

layout(std140) uniform fw64LightingData {
    vec3 fw64_ambient_light_color;
    float fw64_ambient_light_intensity;
    vec3 fw64_light_color;
    float fw64_align1;
    vec3 fw64_light_direction;
    float fw64_align2;
};

layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
};

layout(location = 0) in vec4 vertex_position;
layout(location = 1) in vec3 vertex_normal;
layout(location = 2) in vec2 vertex_tex_coord;

out vec4 calculated_color;

uniform vec4 diffuse_color;

void main() {
    vec3 normal = normalize(mat3(fw64_normal_matrix) * vertex_normal);
    vec3 light_dir = normalize(fw64_light_direction);
    float diff = max(dot(normal, light_dir), 0.0f);

    calculated_color = vec4(fw64_ambient_light_color, 1.0) * fw64_ambient_light_intensity;
    calculated_color += vec4(fw64_light_color * diff, 1.0) * diffuse_color;

    gl_Position = fw64_mvp_matrix * vertex_position;
}