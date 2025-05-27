#include <crimson/renderer.d/VertexArray.hpp>

namespace ct {

VertexArray::VertexArray(std::vector<VertexArray::Attribute> attributes) {
	using namespace mstd;

	vboCount = attributes.size();

	mstd::alloc(vbos, attributes.size());

	glCreateBuffers(attributes.size(), vbos);
	glCreateBuffers(1, &ebo);
	glCreateVertexArrays(1, &vao);

	glNamedBufferData(vbos[0], 0, nullptr, GL_STREAM_DRAW);
	glNamedBufferData(ebo, 0, nullptr, GL_STREAM_DRAW);

	for (U32 i = 0; i < attributes.size(); ++i) {
		glEnableVertexArrayAttrib(vao, i);
		glVertexArrayAttribBinding(vao, i, i);
		glVertexArrayAttribFormat(
			vao,
			i,
			attributes[i].size,
			attributes[i].type,
			attributes[i].normalized,
			0L
		);
	}

	for (Size i = 0; i < attributes.size(); ++i) {
		glVertexArrayVertexBuffer(vao, i, vbos[i], 0L, attributes[i].stride);
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
