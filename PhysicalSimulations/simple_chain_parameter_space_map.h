#pragma once

#include "texture.h"
#include <vector>
#include "frame_buffer.h"
#include "buffer.h"
#include "vertex_array.h"
#include "shader.h"

class SimpleChain;
class SimpleChainParameterSpaceMap {
	static constexpr int SIZE = 360;
	Texture tex_map;
	FrameBuffer fbo;
	VertexArray vao;
	VertexBuffer vbo;
	ShaderStorageBuffer ssbo;

	Shader shader;
	GLint l1_location, l2_location;
	GLint obstacle_count_location;

public:
	SimpleChainParameterSpaceMap();

	void render(const SimpleChain& chain);
	std::vector<Vector2> seek_path(const Vector2& from, const Vector2& to);
	void* get_texture_pointer() const { return (void*)(intptr_t)tex_map.get_id(); }
};