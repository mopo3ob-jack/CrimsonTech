#include "../../include/render/TextureHandler.hpp"

#include <stdio.h>
#include <vector>

namespace hdb {

namespace ct {

TextureHandler::TextureHandler() {
	glCreateBuffers(1, &handleBuffer);
}

TextureHandler::TextureHandler(unsigned int count) {
	glCreateBuffers(1, &handleBuffer);

	hdb::alloc(textureBuffer, count);
	textureCount = count;
	glCreateTextures(GL_TEXTURE_2D, textureCount, textureBuffer);

	for (unsigned int i = 0; i < textureCount; ++i) {
		textureBuffer[i] = -1;
	}
}

TextureHandler::~TextureHandler() {
	if (handleBuffer != -1) {
		glDeleteBuffers(1, &handleBuffer);
		handleBuffer = -1;
	}

	if (textureBuffer) {
		glDeleteTextures(textureCount, textureBuffer);
		hdb::free(textureBuffer);
		textureBuffer = nullptr;
		textureCount = 0;
	}
}

void TextureHandler::makeTexturesResident() {
	for (unsigned int i = 0; i < textureCount; ++i) {
		glMakeTextureHandleResidentARB(glGetTextureHandleARB(textureBuffer[i]));
	}
}

void TextureHandler::makeTexturesNonResident() {
	for (unsigned int i = 0; i < textureCount; ++i) {
		glMakeTextureHandleNonResidentARB(glGetTextureHandleARB(textureBuffer[i]));
	}
}

void TextureHandler::resize(unsigned int count) {
	hdb::resize(textureBuffer, count);

	static std::vector<unsigned long> handleTempBuffer;
	handleTempBuffer.resize(count);
	for (uint i = 0; i < textureCount; ++i) {
		handleTempBuffer[i] = glGetTextureHandleARB(textureBuffer[i]);
	}

	for (uint i = textureCount; i < count; ++i) {
		handleTempBuffer.at(i) = -1; 
	}

	glNamedBufferStorage(handleBuffer, count * sizeof(unsigned long), handleTempBuffer.data(), GL_DYNAMIC_STORAGE_BIT);

	for (uint i = this->textureCount; i < count; ++i) {
		textureBuffer[i] = -1;
	}

	this->textureCount = count;
}

void TextureHandler::updateTexture(unsigned int index, unsigned char* pixels, unsigned int width, unsigned int height, unsigned int mips) {
	unsigned int& texture = textureBuffer[index];
	
	if (texture != -1)
		glDeleteTextures(1, &texture);

	glCreateTextures(GL_TEXTURE_2D, 1, &texture);

	glTextureParameteri(texture, GL_TEXTURE_WRAP_S, GL_MIRRORED_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_WRAP_T, GL_MIRRORED_REPEAT);
	glTextureParameteri(texture, GL_TEXTURE_MIN_FILTER, GL_LINEAR_MIPMAP_LINEAR);
	glTextureParameteri(texture, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	if (mips == -1) {
		mips = sizeof(unsigned int) * 8 - __builtin_clzll(width) - 1;
	}

	glTextureStorage2D(texture, mips, GL_RGB8, width, height);
	glTextureSubImage2D(texture, 0, 0, 0, width, height, GL_RGB, GL_UNSIGNED_BYTE, pixels);
	glGenerateTextureMipmap(texture);

	unsigned long handle = glGetTextureHandleARB(texture);
	glNamedBufferSubData(handleBuffer, index * sizeof(unsigned long), sizeof(unsigned long), &handle);
	glBindBufferBase(GL_SHADER_STORAGE_BUFFER, 0, handleBuffer);
	// glNamedBufferStorage(handleBuffer, sizeof(unsigned long), &handle, GL_DYNAMIC_STORAGE_BIT);
}

}
}
