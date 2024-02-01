#ifndef HDB_CT_RENDERBATCH_HPP
#define HDB_CT_RENDERBATCH_HPP

#include "../geometry/Vertex.hpp"
#include "../../glad/include/glad/glad.h"
#include "../utils/utils.hpp"

namespace hdb {

namespace ct {

class RenderBatch {
public:
	RenderBatch();

	RenderBatch(unsigned int elementBufferCount);

	~RenderBatch();

	RenderBatch(const RenderBatch&) = delete;
	RenderBatch operator=(const RenderBatch&) = delete;

	void allocateBuffers(unsigned int elementBufferCount);
	inline void vertexData(Vertex* vertices, unsigned int count, GLenum usage) {
		glNamedBufferData(vertexBuffer, count * sizeof(Vertex), vertices, usage);
	}
	
	inline void indexData(unsigned int buffer, unsigned int* indices, unsigned int count, GLenum usage) {
		glNamedBufferData(indexBuffers[buffer], count * sizeof(unsigned int), indices, usage);
		bufferSizes[buffer] = count;
	}

	inline void bindVertexBuffer() { glBindBuffer(GL_ARRAY_BUFFER, vertexBuffer); }

	void render(unsigned int buffer);

protected:
	void genNecessities();

	unsigned int* indexBuffers = nullptr;
	unsigned int* bufferSizes = nullptr;
	unsigned int indexBufferCount = 0;
	unsigned int vertexBuffer;
	
	static unsigned int vertexArrayObject;
	static bool isInstantiated;
};

}

}

#endif