#include "shader_library.h"
#include <stdexcept>

bool ShaderLibrary::initialized = false;
Shader ShaderLibrary::shaders[ShaderLibrary::SHADER_COUNT];

void ShaderLibrary::init()
{
	shaders[0].init("simple_vertex_shader.glsl", "simple_fragment_shader.glsl");
	shaders[1].init("axes_vertex_shader.glsl", "axes_fragment_shader.glsl");
	shaders[2].init("phong_vertex_shader.glsl", "phong_fragment_shader.glsl");
	shaders[3].init("phong_deformed_vertex_shader.glsl", "phong_fragment_shader.glsl");

	initialized = true;
}

Shader& ShaderLibrary::get_shader(const ShaderType& type)
{
	if (!initialized)
		throw std::logic_error("Trying to get non-initialized shader");

	return shaders[static_cast<int>(type)];
}

void ShaderLibrary::destroy_shaders()
{
	for (int i = 0; i < SHADER_COUNT; ++i)
		shaders[i].dispose();

	initialized = false;
}
