#version 410 core

layout(location = 0) in vec3 input_pos;
layout(location = 1) in vec3 input_normal;

out vec3 world_pos;
out vec3 normal;

uniform mat4 pv;
uniform mat4 m;

void main() {
	vec4 world4 = m * vec4(input_pos, 1.0f);
	world_pos = world4.xyz;
	normal = normalize((m * vec4(input_normal, 0.0f)).xyz);
	gl_Position = pv * world4;
}