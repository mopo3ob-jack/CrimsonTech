#ifndef CT_VERTEXARRAY_HPP
#define CT_VERTEXARRAY_HPP

#include <crimson/renderer.d/gl.hpp>
#include <vector>
#include <mstd/memory>
#include <type_traits>
#include <span>

namespace ct {

class VertexArray {
public:
	struct Attribute {
		GLsizei stride;
		GLint size;
		GLenum type;
		GLboolean normalized;
		mstd::Bool integral = false;
	};

	VertexArray() {}

	VertexArray(
		std::span<const Attribute> attributes,
		mstd::Size vertexCount,
		mstd::Size elementCount,
		mstd::Size elementSize,
		const void* data = nullptr
	);

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

	mstd::Status load(
		const mstd::C8* inputPath,
		mstd::U32& vertexCount,
		mstd::U32 elementCount,
		mstd::Arena& arena
	);

	struct ConvertOptions {
		mstd::I32 positions = 0;
		mstd::I32 materials = 1;
		mstd::I32 uvs = 2;
		mstd::I32 normals = 3;
		mstd::I32 tangents = 4;
		mstd::I32 bitangents = 5;
	};

	static mstd::Status convert(
		const mstd::C8* inputPath,
		const mstd::C8* outputPath,
		ConvertOptions options,
		mstd::Arena& arena
	);

protected:
	std::vector<GLintptr> attributeOffsets;

	GLuint vbo = 0;
	GLuint ebo = 0;
	GLuint vao = 0;
};

}

#endif
