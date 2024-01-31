#pragma once
#include <glad/glad.h>
#include "algebra.h"

class Shader {
protected:
	GLint color_location;
	GLint pv_location;
	GLint m_location;
	GLint cam_pos_location;

	GLuint load_shader(const char* filename, GLenum shader_type);
	void init_uniform_locations();
public:
	GLuint id;
	void init(const char* vertex_shader_file, const char* fragment_shader_file);
	void init(const char* vertex_shader_file, const char* geometry_shader_file, const char* fragment_shader_file);
	void init(const char* vertex_shader_file, const char* tess_control_shader_file, const char* tess_eval_shader_file, const char* fragment_shader_file);
	void init(const char* vertex_shader_file, const char* tess_control_shader_file, const char* tess_eval_shader_file, const char* geometry_shader_file, const char* fragment_shader_file);
	void use();
	GLuint get_uniform_location(const GLchar* name);
	void set_color(const float& r, const float& g, const float& b, const float& a);
	void set_pv(const Matrix4x4& pv);
	void set_m(const Matrix4x4& m);
	void set_camera_position(const Vector3& position);

	void dispose();
};