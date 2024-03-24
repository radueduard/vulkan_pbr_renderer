#version 450 core

layout (isolines, equal_spacing, ccw) in;

layout (location = 0) out vec3 fragPos;

layout (set = 0, binding = 0) uniform Camera {
    mat4 view;
    mat4 proj;
    mat4 inverseView;
    mat4 inverseProj;
} camera;

layout (set = 1, binding = 0) uniform Config {
    int size;
    int tesselation;
} config;

void main() {
    float u = gl_TessCoord.x;
    float v = gl_TessCoord.y;

    vec4 pos0 = gl_in[0].gl_Position;
    vec4 pos1 = gl_in[1].gl_Position;
    vec4 pos2 = gl_in[2].gl_Position;
    vec4 pos3 = gl_in[3].gl_Position;

    vec4 left = pos0 + v * (pos3 - pos0);
    vec4 right = pos1 + v * (pos2 - pos1);

    vec4 pos = left + u * (right - left);

    mat4 model = mat4(mat3(config.size));

    fragPos = (model * pos).xyz;
    gl_Position = camera.proj * camera.view * model * pos;
}