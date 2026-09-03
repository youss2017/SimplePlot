#version 330

layout (location = 0) in vec2 inPosition;

uniform vec2 x_range;
uniform vec2 y_range;
uniform bool u_normalize_range;

float normalize_range(float v, float v_min, float v_max) {
	float normalized_v = 2.0 * ((v - v_min) / (v_max - v_min)) - 1.0;
	return normalized_v;
};

void main() {
	if (u_normalize_range) {
		gl_Position = vec4(normalize_range(inPosition.x, x_range[0], x_range[1]), normalize_range(inPosition.y, y_range[0], y_range[1]), 0.0, 1.0);
	} else {
		gl_Position = vec4(inPosition.x, inPosition.y, 0.0, 1.0);
	}
}