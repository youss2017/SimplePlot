#version 330 core

layout (location = 0) out vec4 FragColor;

in vec2 UV;

uniform sampler2D CurveTexture;

void main() {
	FragColor = texture(CurveTexture, UV);
}