#version 410

uniform vec4 diffuse_color;
out vec4 final_color;

void main() {
    final_color = diffuse_color;
}
