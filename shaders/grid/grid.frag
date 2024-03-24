#version 450 core

layout (location = 0) in vec3 position;

layout (location = 0) out vec4 fragColor;

void main()
{
    bool onXOY = abs(position.z) < 0.001f;
    bool onXOZ = abs(position.y) < 0.001f;
    bool onYOZ = abs(position.x) < 0.001f;

    bool onOX = onXOY && onXOZ;
    bool onOY = onXOY && onYOZ;
    bool onOZ = onXOZ && onYOZ;

    if (onOX)
        fragColor = vec4(1.0f, 0.0f, 0.0f, 1.0f);
    else if (onOY)
        fragColor = vec4(0.0f, 1.0f, 0.0f, 1.0f);
    else if (onOZ)
        fragColor = vec4(0.0f, 0.0f, 1.0f, 1.0f);
    else if (onXOZ)
        fragColor = vec4(1.0f, 1.0f, 1.0f, 1.0f);
    else discard;
}