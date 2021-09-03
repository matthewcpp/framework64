#version 410

in vec4 calculated_color;
out vec4 final_color;

uniform vec4 diffuse_color;

void main() {
    final_color = calculated_color + diffuse_color;
}
