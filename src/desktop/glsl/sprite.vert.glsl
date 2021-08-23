#version 410

precision mediump float;

layout(location = 0) in vec4 fw64_position;
layout(location = 1) in vec2 fw64_tex_coords;

uniform mat4 matrix;

out vec2 tex_coords;

void main() {
    tex_coords = vec2(fw64_tex_coords.x, 1.0 - fw64_tex_coords.y);
    gl_Position = matrix * fw64_position;
}