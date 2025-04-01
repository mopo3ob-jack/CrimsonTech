#ifndef CT_VERTEXARRAY_HPP
#define CT_VERTEXARRAY_HPP

#include <glad/glad.h>
#include <vector>
#include <mstd/misc>
#include <cstring>

namespace ct {

class VertexArray {
public:
	struct Attribute {
		GLint size;
		GLenum type;
		GLboolean normalized;
		GLsizei stride;
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
		}
	}

private:
	GLuint* vbos;
	mstd::Size vboCount;
	GLuint ebo = 0;
	GLuint vao = 0;
};

}

#endif
