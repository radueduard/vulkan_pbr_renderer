#version 450 core

layout (vertices = 4) out;

layout (set = 1, binding = 0) uniform Config {
    int size;
    int tesselation;
} config;

void main() {
    if (gl_InvocationID == 0) {
        gl_TessLevelInner[0] = float(config.tesselation);
        gl_TessLevelInner[1] = float(config.tesselation);

        gl_TessLevelOuter[0] = float(config.tesselation);
        gl_TessLevelOuter[1] = float(config.tesselation);
        gl_TessLevelOuter[2] = float(config.tesselation);
        gl_TessLevelOuter[3] = float(config.tesselation);
    }
    gl_out[gl_InvocationID].gl_Position = gl_in[gl_InvocationID].gl_Position;
}