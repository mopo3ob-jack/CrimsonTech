#include "../../include/render/RenderBatch.hpp"

namespace hdb {

namespace ct {

unsigned int RenderBatch::vertexArrayObject;
bool RenderBatch::isInstantiated = false;

RenderBatch::RenderBatch() {
	genNecessities();
}

RenderBatch::RenderBatch(unsigned int indexBufferCount) {
	genNecessities();

	allocateBuffers(indexBufferCount);
}

void RenderBatch::genNecessities() {
	if (!isInstantiated) {
		glCreateVertexArrays(1, &vertexArrayObject);
	}

	glCreateBuffers(1, &vertexBuffer);

	glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBuffer, 0, sizeof(Vertex));

	if (!isInstantiated) {
		glVertexArrayAttribFormat(vertexArrayObject, 0, 3, GL_FLOAT, GL_FALSE, 0);
		glVertexArrayAttribFormat(vertexArrayObject, 1, 2, GL_FLOAT, GL_FALSE, offsetof(Vertex, texCoords));
		glVertexArrayAttribFormat(vertexArrayObject, 2, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, normal));
		glVertexArrayAttribFormat(vertexArrayObject, 3, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, tangent));
		glVertexArrayAttribFormat(vertexArrayObject, 4, 3, GL_FLOAT, GL_FALSE, offsetof(Vertex, bitangent));
		glVertexArrayAttribFormat(vertexArrayObject, 5, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(Vertex, textureID));
		glVertexArrayAttribFormat(vertexArrayObject, 6, 1, GL_UNSIGNED_INT, GL_FALSE, offsetof(Vertex, transformID));
		
		for (unsigned int i = 0; i < 7; ++i) {
			glEnableVertexArrayAttrib(vertexArrayObject, i);
			glVertexArrayAttribBinding(vertexArrayObject, i, 0);
		}

		isInstantiated = true;
	}
}

RenderBatch::~RenderBatch() {
	if (indexBuffers) {
		glDeleteVertexArrays(1, &vertexArrayObject);
		glDeleteBuffers(indexBufferCount, indexBuffers);
		glDeleteBuffers(1, &vertexBuffer);

		hdb::free(indexBuffers);
		hdb::free(bufferSizes);
		
		indexBuffers = nullptr;
		indexBufferCount = 0;
	}

	if (isInstantiated) {
		glDeleteVertexArrays(1, &vertexArrayObject);
		isInstantiated = false;
	}
}

void RenderBatch::allocateBuffers(unsigned int indexBufferCount) {
	if (indexBufferCount > this->indexBufferCount) {
		hdb::resize(indexBuffers, indexBufferCount);
		hdb::resize(bufferSizes, indexBufferCount);

		unsigned int count = indexBufferCount - this->indexBufferCount;
		glCreateBuffers(count, indexBuffers + this->indexBufferCount);
		
		this->indexBufferCount = indexBufferCount;
	} else if (indexBufferCount < this->indexBufferCount - 8) {
		unsigned int count = this->indexBufferCount - indexBufferCount;
		glDeleteBuffers(count, indexBuffers + indexBufferCount);
		
		hdb::resize(indexBuffers, indexBufferCount);
		hdb::resize(bufferSizes, indexBufferCount);
		this->indexBufferCount = indexBufferCount;
	}
}

void RenderBatch::render(unsigned int buffer) {
	glBindVertexArray(vertexArrayObject);
	glVertexArrayVertexBuffer(vertexArrayObject, 0, vertexBuffer, 0, sizeof(Vertex));
	glVertexArrayElementBuffer(vertexArrayObject, indexBuffers[buffer]);
	glDrawElements(GL_TRIANGLES, bufferSizes[buffer], GL_UNSIGNED_INT, nullptr);
}

}

}