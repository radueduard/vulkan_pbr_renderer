#version 450 core

layout (set = 2, binding = 0) uniform Parameters {
    float alphaCutoff;
    bool doubleSided;
    vec3 emissiveFactor;
    vec4 baseColorFactor;
    float roughnessFactor;
    float metallicFactor;
} parameters;

layout (set = 2, binding = 1) uniform sampler2D emissiveTexture;
layout (set = 2, binding = 2) uniform sampler2D normalTexture;
layout (set = 2, binding = 3) uniform sampler2D occlusionTexture;
layout (set = 2, binding = 4) uniform sampler2D baseColorTexture;
layout (set = 2, binding = 5) uniform sampler2D metallicRoughnessTexture;

layout (location = 0) in vec2 texCoord;

layout (location = 0) out vec4 color;

void main() {
	color = texture(baseColorTexture, texCoord);
    if (color.a < parameters.alphaCutoff) discard;
}