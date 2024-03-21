#version 330 core

layout (location = 0) in vec2 inPosition;
layout (location = 1) in vec2 inUV;

float map(float value, float from_min, float from_max, float to_min, float to_max) {
    // Map value from the range [from_min, from_max] to the range [to_min, to_max]
    return (value - from_min) * (to_max - to_min) / (from_max - from_min) + to_min;
}

out vec2 UV;

void main() {
	UV = inUV;
	gl_Position = vec4(map(inPosition.x, 0.0, 1.0, -1.0, 1.0), map(inPosition.y, 0.0, 1.0, -1.0, 1.0), 0.0, 1.0);
}