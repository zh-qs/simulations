#version 430 core

in vec2 input_pos;
out vec2 tex_coord;

void main() {
	gl_Position = vec4(input_pos, 0.0f, 1.0f);
	tex_coord = 0.5f * input_pos + vec2(0.5f, 0.5f);
}