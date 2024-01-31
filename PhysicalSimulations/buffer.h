#pragma once

#include <glad/glad.h>

template <typename T, GLenum TYPE, GLenum TARGET>
class GlBuffer {
	GLuint id;
public:
	void init() {
		glGenBuffers(1, &id);
	}

	void bind() const {
		glBindBuffer(TARGET, id);
	}

	void unbind() const {
		glBindBuffer(TARGET, 0);
	}

	void set_static_data(const T* data, GLsizeiptr data_size) {
		glBufferData(TARGET, data_size, data, GL_STATIC_DRAW);
	}

	void set_dynamic_data(const T* data, GLsizeiptr data_size) {
		glBufferData(TARGET, data_size, data, GL_DYNAMIC_DRAW);
	}

	void attrib_buffer(GLuint index, GLint size) const {
		glEnableVertexAttribArray(index);
		glVertexAttribPointer(index, size, TYPE, GL_FALSE, 0, nullptr);
	}

	void bind_base(GLuint index) const {
		glBindBufferBase(TARGET, index, id);
	}

	void dispose() {
		glDeleteBuffers(1, &id);
	}
};

using VertexBuffer = GlBuffer<GLfloat, GL_FLOAT, GL_ARRAY_BUFFER>;
using ElementBuffer = GlBuffer<GLuint, GL_UNSIGNED_INT, GL_ELEMENT_ARRAY_BUFFER>;
using ShaderStorageBuffer = GlBuffer<GLfloat, GL_FLOAT, GL_SHADER_STORAGE_BUFFER>;