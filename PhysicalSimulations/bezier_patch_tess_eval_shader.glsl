#version 410 core

layout(quads, equal_spacing) in;

uniform mat4 pv;
uniform mat4 m;

out vec3 world_pos;
out vec3 normal;

vec4 de_casteljau3(vec4 b0, vec4 b1, vec4 b2, vec4 b3, float t)
{
	b0 = (1 - t) * b0 + t * b1;
	b1 = (1 - t) * b1 + t * b2;
	b2 = (1 - t) * b2 + t * b3;

	b0 = (1 - t) * b0 + t * b1;
	b1 = (1 - t) * b1 + t * b2;

	b0 = (1 - t) * b0 + t * b1;

	return b0;
}

vec4 de_casteljau2(vec4 b0, vec4 b1, vec4 b2, float t)
{
	b0 = (1 - t) * b0 + t * b1;
	b1 = (1 - t) * b1 + t * b2;

	b0 = (1 - t) * b0 + t * b1;

	return b0;
}

void main()
{
	float u = gl_TessCoord.x, v = gl_TessCoord.y;

	vec4 p0 = de_casteljau3(gl_in[0].gl_Position, gl_in[1].gl_Position, gl_in[2].gl_Position, gl_in[3].gl_Position, u);
	vec4 p1 = de_casteljau3(gl_in[4].gl_Position, gl_in[5].gl_Position, gl_in[6].gl_Position, gl_in[7].gl_Position, u);
	vec4 p2 = de_casteljau3(gl_in[8].gl_Position, gl_in[9].gl_Position, gl_in[10].gl_Position, gl_in[11].gl_Position, u);
	vec4 p3 = de_casteljau3(gl_in[12].gl_Position, gl_in[13].gl_Position, gl_in[14].gl_Position, gl_in[15].gl_Position, u);

	vec4 dp0 = 3.0f * de_casteljau2(gl_in[1].gl_Position - gl_in[0].gl_Position, gl_in[2].gl_Position - gl_in[1].gl_Position, gl_in[3].gl_Position - gl_in[2].gl_Position, u);
	vec4 dp1 = 3.0f * de_casteljau2(gl_in[5].gl_Position - gl_in[4].gl_Position, gl_in[6].gl_Position - gl_in[5].gl_Position, gl_in[7].gl_Position - gl_in[6].gl_Position, u);
	vec4 dp2 = 3.0f * de_casteljau2(gl_in[9].gl_Position - gl_in[8].gl_Position, gl_in[10].gl_Position - gl_in[9].gl_Position, gl_in[11].gl_Position - gl_in[10].gl_Position, u);
	vec4 dp3 = 3.0f * de_casteljau2(gl_in[13].gl_Position - gl_in[12].gl_Position, gl_in[14].gl_Position - gl_in[13].gl_Position, gl_in[15].gl_Position - gl_in[14].gl_Position, u);

	vec4 du = de_casteljau3(dp0, dp1, dp2, dp3, v);
	vec4 dv = 3.0f * de_casteljau2(p1 - p0, p2 - p1, p3 - p2, v);
	vec4 cr4 = m * vec4(cross(du.xyz, dv.xyz), 0.0f);
	normal = normalize(cr4.xyz);

	vec4 world4 = m * de_casteljau3(p0, p1, p2, p3, v);
	world_pos = world4.xyz;
	gl_Position = pv * world4;
}