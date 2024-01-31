#pragma once

#include "mesh.h"
#include "bezier_cube.h"

template <GLenum MODE>
class DeformedMesh : public Mesh<MODE> {
	GLint bezier_location;
	GLint normal_mode_location;
	GLint normal_scale_location;
public:
	DeformedMesh() : Mesh<MODE>(ShaderType::PhongDeformed) {
		Shader& shader = ShaderLibrary::get_shader(ShaderType::PhongDeformed);
		bezier_location = shader.get_uniform_location("bezier");
		normal_mode_location = shader.get_uniform_location("normal_mode");
		normal_scale_location = shader.get_uniform_location("normal_scale");
	}

	void render(const Camera& camera, int width, int height, const Vector3* bezier, const int normal_mode = 0, const float normal_scale = 1.1f);
};

template<GLenum MODE>
void DeformedMesh<MODE>::render(const Camera& camera, int width, int height, const Vector3* bezier, const int normal_mode, const float normal_scale)
{
	if (!this->visible)
		return;

	auto pv = camera.get_projection_matrix(width, height) * camera.get_view_matrix();

	glEnable(GL_CULL_FACE);

	Shader& shader = ShaderLibrary::get_shader(this->shader_type);

	shader.use();
	shader.set_pv(pv);
	shader.set_m(this->model);
	shader.set_color(this->color.x, this->color.y, this->color.z, this->color.w);
	shader.set_camera_position(camera.get_world_position());
	glUniform3fv(bezier_location, CUBE_POINT_COUNT, reinterpret_cast<const float*>(bezier));
	glUniform1i(normal_mode_location, normal_mode);
	glUniform1f(normal_scale_location, normal_scale);

	this->vao.bind();
	glDrawElements(MODE, this->indices_count, GL_UNSIGNED_INT, nullptr);
	//glDrawArrays(MODE, 0, point_count);
	this->vao.unbind();
}

using DeformedTriMesh = DeformedMesh<GL_TRIANGLES>;
