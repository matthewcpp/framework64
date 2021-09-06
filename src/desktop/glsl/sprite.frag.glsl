precision mediump float;

out vec4 final_color;

in vec2 tex_coords;
uniform sampler2D sampler;

void main() {
    final_color = texture(sampler, tex_coords);
}