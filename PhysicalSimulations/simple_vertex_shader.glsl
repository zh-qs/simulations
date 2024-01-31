#version 410 core

in vec3 input_pos;

uniform mat4 pv;
uniform mat4 m;

void main() {
	vec4 world4 = m * vec4(input_pos, 1.0f);
	gl_Position = pv * world4;
}