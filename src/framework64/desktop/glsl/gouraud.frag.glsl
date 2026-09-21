precision highp float;

in vec4 calculated_color;
out vec4 final_color;

layout(std140) uniform fw64FogData {
    vec4 fw64_fog_color;
    float fw64_fog_min;
    float fw64_fog_max;
    float fw64_fog_cam_near;
    float fw64_fog_cam_far;
    float fw64_fog_enabled;
};

/** Computes the amount of fog color to mix into the final pixel color. 
    The value is based on the non-linear depth distance between the near and far plane
    This attempts to approximate the fog algorithm for n64_libultra.
*/
float fw64_get_fog_factor() {
    float fog_value = gl_FragCoord.z - fw64_fog_min;
    return clamp(fog_value / (fw64_fog_max - fw64_fog_min), 0.0, 1.0) * fw64_fog_enabled;
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

    final_color = mix(final_color, fw64_fog_color, fw64_get_fog_factor());

    if (final_color.a == 0.0)
        discard;
}
