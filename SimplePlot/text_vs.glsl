#version 330 core

layout (location = 0) in vec4 inVertex;
out vec2 TexCoords;

uniform vec2 Resolution;

void main() {
    TexCoords = inVertex.zw;

    // Convert screen coordinates (inVertex.xy) to normalized device coordinates
    vec2 normalizedCoords = (inVertex.xy / Resolution) * 2.0 - 1.0;
    gl_Position = vec4(normalizedCoords, 0.0, 1.0);
}
