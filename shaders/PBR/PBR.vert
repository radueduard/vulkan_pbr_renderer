#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec3 normal;
layout (location = 2) in vec4 tangent;

layout (location = 3) in vec2 tex_coord_0;
layout (location = 4) in vec2 tex_coord_1;

layout (location = 5) in vec4 color_0;

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 inverseView;
    mat4 inverseProj;
} camera;

layout (set = 1, binding = 0) uniform Object{
    mat4 model;
} object;
layout (location = 0) out vec2 tex_coord;

void main()
{
    tex_coord = tex_coord_0;
	gl_Position = camera.proj * camera.view * object.model * vec4(position, 1.0);
}