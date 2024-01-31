#version 410 core

in vec3 input_pos;

void main() {
	gl_Position = vec4(input_pos, 1.0f);
}