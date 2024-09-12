// This should be kept in sync with include/framework64/renderer.h
#define FW64_MAX_LIGHT_COUNT 3

struct fw64Light {
    vec4 light_color;
    /// this direction is the vector TO the light in camera space
    vec4 light_direction;
};

layout(std140) uniform fw64LightingData {
    vec4 fw64_ambient_light_color;
    fw64Light fw64_lights[FW64_MAX_LIGHT_COUNT];
    int fw64_active_light_count;
};

layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
    float fw64_camera_near;
    float fw64_camera_far;
};

layout(location = 0) in vec4 fw64_vertex_position;
layout(location = 1) in vec3 fw64_vertex_normal;

#ifdef FW64_DIFFUSE_TEXTURE
layout(location = 2) in vec2 fw64_vertex_tex_coord;
out vec2 tex_coords;
#endif

out vec4 calculated_color;

uniform vec4 diffuse_color;

void main() {
    vec3 normal = normalize(mat3(fw64_normal_matrix) * fw64_vertex_normal);
    vec4 gouraud_color = fw64_ambient_light_color;

    for (int i = 0; i < fw64_active_light_count; i++) {
        vec3 light_dir = normalize(fw64_lights[i].light_direction.xyz);
        float diffuse = max(dot(normal, light_dir), 0.0f);
        gouraud_color += (diffuse * diffuse_color) * fw64_lights[i].light_color;
    }

    calculated_color = min(gouraud_color, vec4(1.0));

    #ifdef FW64_DIFFUSE_TEXTURE
    tex_coords = fw64_vertex_tex_coord;
    #endif

    gl_Position = fw64_mvp_matrix * fw64_vertex_position;
}