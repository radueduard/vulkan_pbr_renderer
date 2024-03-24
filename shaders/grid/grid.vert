#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 tangent;

layout (location = 3) in vec2 tex_coord_0;
layout (location = 4) in vec2 tex_coord_1;

layout (location = 5) in vec3 color_0;

layout (set = 1, binding = 0) uniform Config {
    int size;
    int tesselation;
} config;

void main()
{
    gl_Position = vec4(position, 1.0);
}