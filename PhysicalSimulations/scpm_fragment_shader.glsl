#version 430 core

in vec2 tex_coord;
out vec4 output_color;

const float TWO_PI = 2.0f * 3.14159265358979323846f;

struct Obstacle {
	vec4 first_corner;
	vec4 second_corner;
	vec4 color;
};

layout(std430, binding = 1) buffer obstacles_data {
	Obstacle obstacles[];
};

uniform float l1, l2;
uniform int obstacle_count;

vec2 get_base() {
	return vec2(0.0f, 0.0f);
}

vec2 get_joint(float angle) {
	return vec2(cos(angle) * l1,
		sin(angle) * l1);
}

vec2 get_effector(vec2 angles) {
	return vec2(cos(angles.x) * l1 + cos(angles.x + angles.y) * l2,
		sin(angles.x) * l1 + sin(angles.x + angles.y) * l2);
}

bool contains(vec2 p, vec2 m, vec2 M) {
	return m.x <= p.x && p.x <= M.x && m.y <= p.y && p.y <= M.y;
}

bool intersects_segment(vec2 start, vec2 end, vec2 m, vec2 M) {
	if (contains(start, m, M) || contains(end, m, M))
		return true;

	if (start.x != end.x)
	{
		float t1 = (m.x - start.x) / (end.x - start.x),
			t2 = (M.x - start.x) / (end.x - start.x);
		if (t1 >= 0 && t1 <= 1)
		{
			float y = start.y + t1 * (end.y - start.y);
			if (y > m.y && y < M.y)
				return true;
		}
		if (t2 >= 0 && t2 <= 1)
		{
			float y = start.y + t2 * (end.y - start.y);
			if (y > m.y && y < M.y)
				return true;
		}
	}
	if (start.y != end.y)
	{
		float t1 = (m.y - start.y) / (end.y - start.y),
			t2 = (M.y - start.y) / (end.y - start.y);
		if (t1 >= 0 && t1 <= 1)
		{
			float x = start.x + t1 * (end.x - start.x);
			if (x > m.x && x < M.x)
				return true;
		}
		if (t2 >= 0 && t2 <= 1)
		{
			float x = start.x + t2 * (end.x - start.x);
			if (x > m.x && x < M.x)
				return true;
		}
	}
	return false;
}

void main() {
	vec2 angles = tex_coord * TWO_PI;
	vec2 b = get_base();
	vec2 j = get_joint(angles.x);
	vec2 p = get_effector(angles);
	for (int i = obstacle_count - 1; i >= 0; --i)
	{
		vec4 m = min(obstacles[i].first_corner, obstacles[i].second_corner),
			M = max(obstacles[i].first_corner, obstacles[i].second_corner);
		if (intersects_segment(b,j,m.xy,M.xy) || intersects_segment(j,p,m.xy,M.xy))
		{
			output_color = obstacles[i].color;
			return;
		}
	}
	output_color = vec4(0.0f, 0.0f, 0.0f, 0.0f);
}