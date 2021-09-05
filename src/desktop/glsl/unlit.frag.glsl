#version 410

in vec4 vertex_color;
out vec4 final_color;

void main() {
    final_color = vertex_color;
}
