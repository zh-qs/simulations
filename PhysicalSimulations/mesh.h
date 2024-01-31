#pragma once

#include "vertex_array.h"
#include "buffer.h"
#include "texture.h"
#include "frame_buffer.h"
#include "algebra.h"
#include "quaternion.h"
#include "camera.h"
#include "shader_library.h"
#include <vector>

template <GLenum MODE>
class Mesh {
protected:
	VertexArray vao;
	VertexBuffer vbo;
	VertexBuffer normal_vbo;
	ElementBuffer ebo;
	ShaderType shader_type;
	size_t indices_count = 0;
	bool has_normals = false;
public:
	Matrix4x4 model = Matrix4x4::identity();
	Vector4 color = { 1.0f,1.0f,1.0f,1.0f };
	bool visible = true;

	explicit Mesh(const ShaderType type = ShaderType::Simple) : vao(), vbo(), shader_type(type) {
		vao.init();
		vao.bind();
		vbo.init();
		vbo.bind();
		/*glEnableVertexAttribArray(0);
		glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, 0, nullptr);*/
		vbo.attrib_buffer(0, 3);
		if (type == ShaderType::Phong || type == ShaderType::PhongDeformed) {
			has_normals = true;
			normal_vbo.init();
			normal_vbo.bind();
			normal_vbo.attrib_buffer(1, 3);
			/*glEnableVertexAttribArray(1);
			glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, 0, nullptr);*/
		}
		ebo.init();
		ebo.bind();
		vao.unbind();

		/*switch (type)
		{
		case ShaderType::Simple:
			shader.init("simple_vertex_shader.glsl", "simple_fragment_shader.glsl");
			break;
		case ShaderType::Axes:
			shader.init("axes_vertex_shader.glsl", "axes_fragment_shader.glsl");
			break;
		case ShaderType::Phong:
			shader.init("phong_vertex_shader.glsl", "phong_fragment_shader.glsl");
			break;
		case ShaderType::PhongDeformed:
			shader.init("phong_deformed_vertex_shader.glsl", "phong_fragment_shader.glsl");
		}*/
		
	}

	~Mesh() {
		ebo.dispose();
		vbo.dispose();
		normal_vbo.dispose();
		vao.dispose();
	}

	template <class I>
	void set_data(const std::vector<Vector3>& points, const std::vector<I>& indices) {
		vbo.bind();
		vbo.set_static_data(reinterpret_cast<const float*>(points.data()), points.size() * sizeof(Vector3));
		ebo.bind();
		ebo.set_static_data(reinterpret_cast<const unsigned int*>(indices.data()), indices.size() * sizeof(I));
		indices_count = indices.size() * sizeof(I) / sizeof(unsigned int);
	}
	void set_data(const std::vector<Vector3>& points);
	void set_normals(const std::vector<Vector3>& normals);
	void render(const Camera& camera, int width, int height);
};

template <GLenum MODE>
void Mesh<MODE>::set_data(const std::vector<Vector3>& points)
{
	std::vector<unsigned int> indices(points.size());
	for (int i = 0; i < indices.size(); ++i)
		indices[i] = i;
	set_data(points, indices);
}

template <GLenum MODE>
void Mesh<MODE>::set_normals(const std::vector<Vector3>& normals)
{
	if (!has_normals)
		return;

	normal_vbo.bind();
	normal_vbo.set_static_data(reinterpret_cast<const float*>(normals.data()), normals.size() * sizeof(Vector3));
}

template <GLenum MODE>
void Mesh<MODE>::render(const Camera& camera, int width, int height)
{
	if (!visible)
		return;

	auto pv = camera.get_projection_matrix(width, height) * camera.get_view_matrix();

	glEnable(GL_CULL_FACE);

	Shader& shader = ShaderLibrary::get_shader(shader_type);
	
	shader.use();
	shader.set_pv(pv);
	shader.set_m(model);
	shader.set_color(color.x, color.y, color.z, color.w);
	shader.set_camera_position(camera.get_world_position());

	vao.bind();
	glDrawElements(MODE, indices_count, GL_UNSIGNED_INT, nullptr);
	//glDrawArrays(MODE, 0, point_count);
	vao.unbind();
}

using LineMesh = Mesh<GL_LINES>;
using LineStripMesh = Mesh<GL_LINE_STRIP>;
using TriMesh = Mesh<GL_TRIANGLES>;
