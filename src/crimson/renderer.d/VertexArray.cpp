#include <crimson/renderer.d/VertexArray.hpp>

namespace ct {

VertexArray::VertexArray(
	const std::vector<VertexArray::Attribute>& attributes,
	mstd::Size vertexCount,
	mstd::Size elementSize
) {
	using namespace mstd;

	attributeOffsets.resize(attributes.size());

	glCreateVertexArrays(1, &vao);
	glCreateBuffers(1, &vbo);
	glCreateBuffers(1, &ebo);
	
	Size vertexSize = 0;
	for (const auto& i : attributes) {
		vertexSize += i.stride;
	}

	glNamedBufferStorage(vbo, vertexCount * vertexSize, nullptr, GL_DYNAMIC_STORAGE_BIT);
	glNamedBufferStorage(ebo, elementSize, nullptr, GL_DYNAMIC_STORAGE_BIT);

	glCreateVertexArrays(1, &vao);

	GLintptr offset = 0;
	for (U32 i = 0; i < attributes.size(); ++i) {
		glVertexArrayVertexBuffer(vao, i, vbo, offset, attributes[i].stride);
		glEnableVertexArrayAttrib(vao, i);
		glVertexArrayAttribFormat(
			vao,
			i,
			attributes[i].size,
			attributes[i].type,
			attributes[i].normalized,
			0L
		);
		glVertexArrayAttribBinding(vao, i, i);

		attributeOffsets[i] = offset;
		offset += attributes[i].stride * vertexCount;
	}

	glVertexArrayElementBuffer(vao, ebo);
}

template <>
void VertexArray::draw<mstd::U8>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_BYTE, (void*)(offset * sizeof(mstd::U8)));
}

template <>
void VertexArray::draw<mstd::U16>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_SHORT, (void*)(offset * sizeof(mstd::U16)));
}

template <>
void VertexArray::draw<mstd::U32>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glDrawElements(GL_TRIANGLES, count, GL_UNSIGNED_INT, (void*)(offset * sizeof(mstd::U32)));
}

}
