#version 410 core

layout(location = 0) in vec3 input_pos;
layout(location = 1) in vec3 input_normal;

out vec3 world_pos;
out vec3 normal;

uniform mat4 pv;
uniform mat4 m;
uniform vec3 bezier[64];
uniform int normal_mode;
uniform float normal_scale;

vec3 de_casteljau3(vec3 b0, vec3 b1, vec3 b2, vec3 b3, float t)
{
	b0 = (1 - t) * b0 + t * b1;
	b1 = (1 - t) * b1 + t * b2;
	b2 = (1 - t) * b2 + t * b3;

	b0 = (1 - t) * b0 + t * b1;
	b1 = (1 - t) * b1 + t * b2;

	b0 = (1 - t) * b0 + t * b1;

	return b0;
}

vec3 de_casteljau2(vec3 b0, vec3 b1, vec3 b2, float t)
{
	b0 = (1 - t) * b0 + t * b1;
	b1 = (1 - t) * b1 + t * b2;

	b0 = (1 - t) * b0 + t * b1;

	return b0;
}

vec3 deform_point(vec3 p) {
	vec3[16] patch_points;

	for (int i = 0; i < 16; ++i)
	{
		patch_points[i] = de_casteljau3(bezier[4 * i], bezier[4 * i + 1], bezier[4 * i + 2], bezier[4 * i + 3], p.x);
	}

	vec3 p0 = de_casteljau3(patch_points[0], patch_points[1], patch_points[2], patch_points[3], p.y);
	vec3 p1 = de_casteljau3(patch_points[4], patch_points[5], patch_points[6], patch_points[7], p.y);
	vec3 p2 = de_casteljau3(patch_points[8], patch_points[9], patch_points[10], patch_points[11], p.y);
	vec3 p3 = de_casteljau3(patch_points[12], patch_points[13], patch_points[14], patch_points[15], p.y);

	return de_casteljau3(p0, p1, p2, p3, p.z);
}

vec3 deform_point_and_normal(vec3 p, vec3 n, out vec3 deformed_normal) {
	vec3[16] patch_points;
	vec3[16] dpatch_points;

	for (int i = 0; i < 16; ++i)
	{
		patch_points[i] = de_casteljau3(bezier[4 * i], bezier[4 * i + 1], bezier[4 * i + 2], bezier[4 * i + 3], p.x);
		dpatch_points[i] = 3.0f * de_casteljau2(bezier[4 * i + 1] - bezier[4 * i], bezier[4 * i + 2] - bezier[4 * i + 1], bezier[4 * i + 3] - bezier[4 * i + 2], p.x);
	}

	vec3 dp0 = de_casteljau3(dpatch_points[0], dpatch_points[1], dpatch_points[2], dpatch_points[3], p.y);
	vec3 dp1 = de_casteljau3(dpatch_points[4], dpatch_points[5], dpatch_points[6], dpatch_points[7], p.y);
	vec3 dp2 = de_casteljau3(dpatch_points[8], dpatch_points[9], dpatch_points[10], dpatch_points[11], p.y);
	vec3 dp3 = de_casteljau3(dpatch_points[12], dpatch_points[13], dpatch_points[14], dpatch_points[15], p.y);

	vec3 p0 = de_casteljau3(patch_points[0], patch_points[1], patch_points[2], patch_points[3], p.y);
	vec3 p1 = de_casteljau3(patch_points[4], patch_points[5], patch_points[6], patch_points[7], p.y);
	vec3 p2 = de_casteljau3(patch_points[8], patch_points[9], patch_points[10], patch_points[11], p.y);
	vec3 p3 = de_casteljau3(patch_points[12], patch_points[13], patch_points[14], patch_points[15], p.y);

	vec3 pd0 = 3.0f * de_casteljau2(patch_points[1] - patch_points[0], patch_points[2] - patch_points[1], patch_points[3] - patch_points[2], p.y);
	vec3 pd1 = 3.0f * de_casteljau2(patch_points[5] - patch_points[4], patch_points[6] - patch_points[5], patch_points[7] - patch_points[6], p.y);
	vec3 pd2 = 3.0f * de_casteljau2(patch_points[9] - patch_points[8], patch_points[10] - patch_points[9], patch_points[11] - patch_points[10], p.y);
	vec3 pd3 = 3.0f * de_casteljau2(patch_points[13] - patch_points[12], patch_points[14] - patch_points[13], patch_points[15] - patch_points[14], p.y);

	vec3 dx = de_casteljau3(dp0, dp1, dp2, dp3, p.z);
	vec3 dy = de_casteljau3(pd0, pd1, pd2, pd3, p.z);
	vec3 dz = 3.0f * de_casteljau2(p1 - p0, p2 - p1, p3 - p2, p.z);

	mat3 invtjacobian = transpose(inverse(mat3(dx, dy, dz)));

	deformed_normal = invtjacobian * n;//vec3(dot(dx, n), dot(dy, n), dot(dz, n));

	return de_casteljau3(p0, p1, p2, p3, p.z);
}

void main() {
	switch (normal_mode)
	{
	case 0: // analytically
		world_pos = deform_point_and_normal(input_pos, input_normal, normal);
		normal = normalize(normal);
		break;
	case 1: // average of normals
		world_pos = deform_point(input_pos);
		normal = vec3(0.0f, 0.0f, 0.0f); // TODO
		break;
	case 2: // scaled model
	{
		world_pos = deform_point(input_pos);
		vec3 scaled_pos = deform_point(((2.0f * input_pos - vec3(1.0f, 1.0f, 1.0f)) * normal_scale + vec3(1.0f,1.0f,1.0f)) * 0.5f); // TODO scale along normal vector!
		normal = normalize(scaled_pos - world_pos);
		if (normal_scale < 1)
			normal = -normal;
		break;
	}
	default:
		world_pos = deform_point(input_pos);
		normal = vec3(0.0f, 0.0f, 0.0f); // this should not happen
		break;
	}
	
	gl_Position = pv * vec4(world_pos, 1.0f);
}