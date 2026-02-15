#include <crimson/renderer.d/IndirectVertexArray.hpp>

namespace ct {

IndirectVertexArray::IndirectVertexArray(
	std::span<const Attribute> attributes,
	mstd::Size vertexCount,
	mstd::Size elementCount,
	mstd::Size elementSize,
	mstd::Size indexCount
) : VertexArray(attributes, vertexCount, elementCount, elementSize) {
	using namespace mstd;

	ibo = StorageBuffer((DrawIndex*)nullptr, indexCount);
}

template <>
void IndirectVertexArray::draw<mstd::U8>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ibo);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_BYTE, (void*)(offset * sizeof(mstd::U8)), count, sizeof(DrawIndex));
}

template <>
void IndirectVertexArray::draw<mstd::U16>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ibo);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_SHORT, (void*)(offset * sizeof(mstd::U16)), count, sizeof(DrawIndex));
}

template <>
void IndirectVertexArray::draw<mstd::U32>(GLsizei count, mstd::Size offset) const {
	glBindVertexArray(vao);
	glBindBuffer(GL_DRAW_INDIRECT_BUFFER, ibo);
	glMultiDrawElementsIndirect(GL_TRIANGLES, GL_UNSIGNED_INT, (void*)(offset * sizeof(mstd::U32)), count, sizeof(DrawIndex));
}

}
