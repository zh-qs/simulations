#version 410 core

in vec3 input_pos;
out vec4 axis_color;

uniform mat4 pv;
uniform mat4 m;

void main() {
	vec4 world4 = m * vec4(input_pos, 1.0f);
	gl_Position = pv * world4;
	axis_color = vec4(abs(normalize(input_pos)), 1.0f);
}