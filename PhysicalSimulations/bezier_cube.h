#pragma once

#include "vertex_array.h"
#include "buffer.h"
#include "texture.h"
#include "frame_buffer.h"
#include "algebra.h"
#include "quaternion.h"
#include "camera.h"
#include "shader.h"
#include <vector>

static constexpr int SIDE_DIM = 4;
static constexpr int CUBE_POINT_COUNT = SIDE_DIM * SIDE_DIM * SIDE_DIM;;
static constexpr int CONFIGURATION_SPACE_DIM = 3 * CUBE_POINT_COUNT;
static constexpr int ODE_DIM = 2 * CONFIGURATION_SPACE_DIM;

class BezierCube {
	VertexArray vao;
	VertexBuffer vbo;
	ElementBuffer line_ebo;
	ElementBuffer patch_ebo;
	Shader simple_shader;
	Shader bezier_shader;
	size_t line_count = 0;
	size_t patch_indices_count = 0;

	unsigned int get_idx(int i, int j, int k) {
		return i + SIDE_DIM * j + SIDE_DIM * SIDE_DIM * k;
	}

	void fill_ebos() {
		line_count = SIDE_DIM * (SIDE_DIM - 1) * 3 * SIDE_DIM;

		std::vector<IndexPair> lines;
		lines.reserve(line_count);

		for (int i = 0; i < SIDE_DIM; ++i)
			for (int j = 0; j < SIDE_DIM; ++j)
				for (int k = 0; k < SIDE_DIM; ++k)
				{
					if (i < SIDE_DIM - 1)
						lines.push_back({ get_idx(i,j,k), get_idx(i + 1,j,k) });
					if (j < SIDE_DIM - 1)
						lines.push_back({ get_idx(i,j,k), get_idx(i,j + 1,k) });
					if (k < SIDE_DIM - 1)
						lines.push_back({ get_idx(i,j,k), get_idx(i,j,k + 1) });
				}
		line_ebo.bind();
		line_ebo.set_static_data(reinterpret_cast<const unsigned int*>(lines.data()), lines.size() * sizeof(IndexPair));

		patch_indices_count = 6 * SIDE_DIM * SIDE_DIM;

		std::vector<unsigned int> patch_indices;
		patch_indices.reserve(patch_indices_count);

		for (int i = 0; i < SIDE_DIM; ++i)
			for (int j = 0; j < SIDE_DIM; ++j)
			{
				patch_indices.push_back(get_idx(i, j, 0));
			}
		for (int i = SIDE_DIM - 1; i >= 0; --i)
			for (int j = 0; j < SIDE_DIM; ++j)
			{
				patch_indices.push_back(get_idx(i, j, SIDE_DIM - 1));
			}
		for (int i = SIDE_DIM - 1; i >= 0; --i)
			for (int k = 0; k < SIDE_DIM; ++k)
			{
				patch_indices.push_back(get_idx(i, 0, k));
			}
		for (int i = 0; i < SIDE_DIM; ++i)
			for (int k = 0; k < SIDE_DIM; ++k)
			{
				patch_indices.push_back(get_idx(i, SIDE_DIM - 1, k));
			}
		for (int j = 0; j < SIDE_DIM; ++j)
			for (int k = 0; k < SIDE_DIM; ++k)
			{
				patch_indices.push_back(get_idx(0, j, k));
			}
		for (int j = SIDE_DIM - 1; j >= 0; --j)
			for (int k = 0; k < SIDE_DIM; ++k)
			{
				patch_indices.push_back(get_idx(SIDE_DIM - 1, j, k));
			}
		patch_ebo.bind();
		patch_ebo.set_static_data(patch_indices.data(), patch_indices.size() * sizeof(unsigned int));
	}

public:
	Matrix4x4 model = Matrix4x4::identity();
	Vector4 line_color = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 point_color = { 1.0f,1.0f,1.0f,1.0f };
	Vector4 patch_color = { 1.0f, 0.5f,0.0f,1.0f };
	bool line_visible = true,
		points_visible = true,
		patches_visible = true;

	BezierCube() : vao(), vbo() {
		vao.init();
		vao.bind();
		vbo.init();
		vbo.bind();
		glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);
		line_ebo.init();
		patch_ebo.init();
		vao.unbind();

		fill_ebos();

		simple_shader.init("simple_vertex_shader.glsl", "simple_fragment_shader.glsl");
		bezier_shader.init("dummy_vertex_shader.glsl", "patch_tess_control_shader.glsl", "bezier_patch_tess_eval_shader.glsl", "phong_fragment_shader.glsl");
	}

	~BezierCube() {
		simple_shader.dispose();
		line_ebo.dispose();
		vbo.dispose();
		vao.dispose();
	}

	void set_data(const Vector3* data) {
		vbo.bind();
		vbo.set_static_data(reinterpret_cast<const float*>(data), CUBE_POINT_COUNT * sizeof(Vector3));
	}

	void render_points(const Camera& camera, int width, int height)
	{
		if (!points_visible)
			return;

		auto pv = camera.get_projection_matrix(width, height) * camera.get_view_matrix();

		simple_shader.use();
		simple_shader.set_pv(pv);
		simple_shader.set_m(model);
		simple_shader.set_color(point_color.x, point_color.y, point_color.z, point_color.w);
		simple_shader.set_camera_position(camera.get_world_position());

		vao.bind();
		glPointSize(5.0f);
		glDrawArrays(GL_POINTS, 0, CUBE_POINT_COUNT);
		vao.unbind();
	}

	void render_lines(const Camera& camera, int width, int height)
	{
		if (!line_visible)
			return;

		auto pv = camera.get_projection_matrix(width, height) * camera.get_view_matrix();

		simple_shader.use();
		simple_shader.set_pv(pv);
		simple_shader.set_m(model);
		simple_shader.set_color(line_color.x, line_color.y, line_color.z, line_color.w);
		simple_shader.set_camera_position(camera.get_world_position());

		vao.bind();
		line_ebo.bind();
		glDrawElements(GL_LINES, 2 * line_count, GL_UNSIGNED_INT, nullptr);
		vao.unbind();
	}

	void render_bezier_patches(const Camera& camera, int width, int height)
	{
		if (!patches_visible)
			return;

		auto pv = camera.get_projection_matrix(width, height) * camera.get_view_matrix();

		bezier_shader.use();
		bezier_shader.set_pv(pv);
		bezier_shader.set_m(model);
		bezier_shader.set_color(patch_color.x, patch_color.y, patch_color.z, patch_color.w);
		bezier_shader.set_camera_position(camera.get_world_position());

		vao.bind();
		patch_ebo.bind();
		glPatchParameteri(GL_PATCH_VERTICES, 16);
		glDrawElements(GL_PATCHES, patch_indices_count, GL_UNSIGNED_INT, NULL);
		vao.unbind();
	}
};