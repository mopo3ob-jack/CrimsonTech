#ifndef CT_VERTEXARRAY_HPP
#define CT_VERTEXARRAY_HPP

#include <glad/glad.h>
#include <vector>
#include <mstd/memory>
#include <type_traits>

namespace ct {

class VertexArray {
public:
	struct Attribute {
		GLsizei stride;
		GLint size;
		GLenum type;
		GLboolean normalized;
	};

	VertexArray() {}

	VertexArray(std::vector<Attribute> attributes);

	VertexArray(VertexArray& vertexArray) = delete;
	VertexArray& operator=(VertexArray& vertexArray) = delete;

	VertexArray(VertexArray&& vertexArray) = default;
	VertexArray& operator=(VertexArray&& vertexArray) {
		this->~VertexArray();

		this->vbos = vertexArray.vbos;
		this->vboCount = vertexArray.vboCount;
		this->ebo = vertexArray.ebo;
		this->vao = vertexArray.vao;

		vertexArray.vbos = nullptr;
		vertexArray.vboCount = 0L;
		vertexArray.ebo = 0;
		vertexArray.vao = 0;

		return *this;
	}

	~VertexArray() {
		if (vao) {
			glDeleteBuffers(vboCount, vbos);
			glDeleteBuffers(1, &ebo);
			glDeleteVertexArrays(1, &vao);
			mstd::free(vbos);
		}
	}

	template <typename T>
	void allocateAttributes(mstd::Size attribute, T* data, GLsizeiptr count, GLenum usage = GL_STATIC_DRAW) {
		glNamedBufferData(vbos[attribute], count * sizeof(T), (void*)data, usage);
	}

	template <typename T>
	void writeAttributes(mstd::Size attribute, T* data, GLsizeiptr count, GLintptr offset = 0L) {
		glNamedBufferSubData(vbos[attribute], offset, count * sizeof(T), (void*)data);
	}

	template <typename T> requires std::is_integral_v<T>
	void allocateElements(T* data, GLsizeiptr count, GLenum usage = GL_STATIC_DRAW) {
		glNamedBufferData(ebo, count * sizeof(T), (void*)data, usage);
	}

	template <typename T> requires std::is_integral_v<T>
	void writeElements(T* data, GLsizeiptr count, GLintptr offset = 0L) {
		glNamedBufferSubData(ebo, offset, count * sizeof(T), (void*)data);
	}

	template <typename T> requires std::is_integral_v<T>
	void draw(GLsizei count, mstd::Size offset) const;

private:
	GLuint* vbos;
	mstd::Size vboCount;
	GLuint ebo = 0;
	GLuint vao = 0;
};

}

#endif
