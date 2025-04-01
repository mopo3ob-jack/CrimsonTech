#include <crimson/renderer.d/VertexArray.hpp>

namespace ct {

VertexArray::VertexArray(std::vector<VertexArray::Attribute> attributes) {
	using namespace mstd;

	vboCount = attributes.size();

	mstd::alloc(vbos, attributes.size());

	glCreateBuffers(attributes.size(), vbos);
	glCreateBuffers(1, &ebo);
	glCreateVertexArrays(1, &vao);

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
}

}
