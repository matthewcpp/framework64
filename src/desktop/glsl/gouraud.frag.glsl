#version 410

in vec4 calculated_color;
out vec4 final_color;

void main() {
    final_color = calculated_color;
}
