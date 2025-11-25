#ifndef CT_INDIRECTVERTEXARRAY_HPP
#define CT_INDIRECTVERTEXARRAY_HPP

#include <crimson/renderer.d/VertexArray.hpp>
#include <crimson/renderer.d/StorageBuffer.hpp>
#include <span>

namespace ct {

class IndirectVertexArray : public VertexArray {
public:
	struct DrawIndex {
		GLuint count;
		GLuint instanceCount;
		GLuint firstVertex;
		GLuint baseVertex;
		GLuint baseInstance;
	};

	IndirectVertexArray() {}

	IndirectVertexArray(
		std::span<const Attribute> attributes,
		mstd::Size vertexCount,
		mstd::Size elementCount,
		mstd::Size elementSize,
		mstd::Size indexCount
	);

	IndirectVertexArray(IndirectVertexArray& vertexArray) = delete;
	IndirectVertexArray& operator=(IndirectVertexArray& vertexArray) = delete;

	IndirectVertexArray(IndirectVertexArray&& vertexArray) {
		*this = std::move(vertexArray);
	}

	IndirectVertexArray& operator=(IndirectVertexArray&& vertexArray) {
		this->~IndirectVertexArray();

		this->attributeOffsets = std::move(vertexArray.attributeOffsets);
		this->vbo = vertexArray.vbo;
		this->ebo = vertexArray.ebo;
		this->vao = vertexArray.vao;
		this->ibo = std::move(vertexArray.ibo);

		vertexArray.vao = 0;

		return *this;
	}

	~IndirectVertexArray() {}

	void writeIndices(DrawIndex* data, GLsizeiptr count, GLintptr offset = 0L) {
		ibo.write(data, count, offset);
	}

	template <typename T> requires std::is_integral_v<T>
	void draw(GLsizei count, mstd::Size offset) const;

private:
	StorageBuffer ibo;
};

}

#endif
