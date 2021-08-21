#version 410

in vec2 tex_coords;

uniform sampler2D tex_sampler;

out vec4 final_color;

void main() {
    final_color = texture(tex_sampler, tex_coords);
}