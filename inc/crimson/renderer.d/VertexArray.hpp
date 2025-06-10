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

	VertexArray();

	VertexArray(VertexArray& vertexArray) = delete;
	VertexArray& operator=(VertexArray& vertexArray) = delete;

	VertexArray(VertexArray&& vertexArray) = default;
	VertexArray& operator=(VertexArray&& vertexArray) {
		this->~VertexArray();

		this->attributeOffsets = std::move(vertexArray.attributeOffsets);
		this->vbo = vertexArray.vbo;
		this->ebo = vertexArray.ebo;
		this->vao = vertexArray.vao;

		vertexArray.vao = 0;

		return *this;
	}

	~VertexArray() {
		if (vao) {
			glDeleteBuffers(1, &vbo);
			glDeleteBuffers(1, &ebo);
			glDeleteVertexArrays(1, &vao);
			vao = 0;
		}
	}

	void allocateAttributes(const std::vector<Attribute>& attributes, mstd::Size vertexCount);

	void allocateElements(mstd::Size bufferSize);

	template <typename T>
	void writeAttributes(mstd::Size attribute, T* data, GLsizeiptr count, GLintptr offset = 0L) {
		glNamedBufferSubData(
			vbo,
			attributeOffsets[attribute] + offset * sizeof(T),
			count * sizeof(T),
			(void*)data
		);
	}

	template <typename T> requires std::is_integral_v<T>
	void writeElements(T* data, GLsizeiptr count, GLintptr offset = 0L) {
		glNamedBufferSubData(ebo, offset * sizeof(T), count * sizeof(T), (void*)data);
	}

	template <typename T> requires std::is_integral_v<T>
	void draw(GLsizei count, mstd::Size offset) const;

private:
	std::vector<GLintptr> attributeOffsets;

	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint vao = 0;
};

}

#endif
