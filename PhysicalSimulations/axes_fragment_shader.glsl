#version 410 core

in vec4 axis_color;
out vec4 output_color;

uniform vec4 color;

void main() {
	output_color = color * axis_color;
}