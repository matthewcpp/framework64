in vec4 calculated_color;
out vec4 final_color;

#ifdef FW64_DIFFUSE_TEXTURE
in vec2 tex_coords;
uniform sampler2D diffuse_texture_sampler;
#endif

void main() {
    #ifdef FW64_DIFFUSE_TEXTURE
        final_color = texture(diffuse_texture_sampler, tex_coords) * calculated_color;
    #else
        final_color = calculated_color;
    #endif
}
