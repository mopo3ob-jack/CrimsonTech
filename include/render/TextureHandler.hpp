#ifndef HDB_CT_TEXTURE_HANDLER_HPP
#define HDB_CT_TEXTURE_HANDLER_HPP

#include "../../glad/include/glad/glad.h"
#include "../utils/utils.hpp"

namespace hdb {

namespace ct {

class TextureHandler {
public:
	TextureHandler();

	TextureHandler(unsigned int count);

	~TextureHandler();

	TextureHandler(const TextureHandler&) = delete;
	TextureHandler operator=(const TextureHandler&) = delete;
	void operator=(TextureHandler&& handler) {
		this->textureBuffer = handler.textureBuffer;
		this->textureCount = handler.textureCount;
		this->handleBuffer = handler.handleBuffer;
	}

	void makeTexturesResident();
	void makeTexturesNonResident();
	void bindBuffer() {
		glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, handleBuffer);
	}

	void resize(unsigned int count);
	void updateTexture(unsigned int index, unsigned char* pixels, unsigned int width, unsigned int height, unsigned int mips = -1);

protected:
	unsigned int* textureBuffer = nullptr;
	unsigned int textureCount = 0;

	unsigned int handleBuffer = -1;
};

}

}

#endif