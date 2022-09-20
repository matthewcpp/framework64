in vec4 unlit_color;
out vec4 final_color;


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

    final_color = texture(diffuse_texture_sampler, sample_coords) * unlit_color;
#else
    final_color = unlit_color;
#endif
}
