in vec4 calculated_color;
out vec4 final_color;

layout(std140) uniform fw64MeshTransformData {
    mat4 fw64_mvp_matrix;
    mat4 fw64_normal_matrix;
    float fw64_camera_near;
    float fw64_camera_far;
};

// TODO: these values should represent 0...1 between near and far plane
// TODO: https://community.khronos.org/t/plane-intersection-on-the-fragment-shader/77283
layout(std140) uniform fw64FogData {
    vec4 fw64_fog_color;
    float fw64_fog_min_distance;
    float fw64_fog_max_distance;
};

float fw64_get_fog_factor(float d) {
    float cam_t = (d - fw64_camera_near) / (fw64_camera_far - fw64_camera_near);

    return smoothstep(fw64_fog_min_distance, fw64_fog_max_distance, cam_t);
}

#ifdef FW64_DIFFUSE_TEXTURE
layout(std140) uniform fw64TextureFrameData {
    float fw64_texture_slice_top;
    float fw64_texture_slice_left;
    float fw64_texture_slice_width;
    float fw64_texture_slice_height;
};

in vec2 tex_coords;
uniform sampler2D diffuse_texture_sampler;
#endif

void main() {
#ifdef FW64_DIFFUSE_TEXTURE
    vec2 sample_coords = vec2(
        fw64_texture_slice_left + (tex_coords.x  * fw64_texture_slice_width),
        fw64_texture_slice_top + (tex_coords.y * fw64_texture_slice_height)
    );

    final_color = texture(diffuse_texture_sampler, sample_coords) * calculated_color;
#else
    final_color = calculated_color;
    #endif

    float fragment_distance_to_camera =  gl_FragCoord.z / gl_FragCoord.w;
    final_color = mix(final_color, fw64_fog_color, fw64_get_fog_factor(fragment_distance_to_camera));
}
