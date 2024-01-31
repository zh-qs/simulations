#version 410 core

in vec3 world_pos;
in vec3 normal;

out vec4 output_color;

uniform vec4 color;
uniform vec3 light_pos = vec3(0.0f, 10.0f, 0.0f);
uniform vec3 light_color = vec3(1.0f, 1.0f, 1.0f);
uniform vec3 cam_pos;

const float ambient = 0.1f;
const float diffuse = 0.5f;
const float specular = 0.5f;
const float m = 15.0f;

void main() {
	vec3 l = normalize(light_pos - world_pos);
	vec3 v = normalize(cam_pos - world_pos);
	vec3 r = normalize(reflect(-l, normal));
	output_color = vec4(color.xyz * (ambient + diffuse * max(dot(normal, l), 0)) + specular * light_color * pow(max(dot(r, v), 0), m), color.w);
}