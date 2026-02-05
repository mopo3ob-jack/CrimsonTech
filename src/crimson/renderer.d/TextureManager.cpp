#include <crimson/renderer.d/TextureManager.hpp>
#include <GL/glu.h>
#include <iostream>

namespace ct {

TextureManager::TextureManager(mstd::U32 textureCount[TEXTURE_UNITS], GLuint shaderProgram) {
	using namespace mstd;

	std::copy(textureCount, textureCount + TEXTURE_UNITS, this->textureCount);
	
	glCreateTextures(GL_TEXTURE_2D_ARRAY, TEXTURE_UNITS, handles);
	
	mstd::C8 uniformName[] = "textures[xxx]";

	U32 width = MIN_TEXTURE_WIDTH;
	for (U32 i = 0; i < TEXTURE_UNITS; ++i) {
		if (!textureCount[i]) {
			width <<= 1;
			continue;
		}

		glTextureParameteri(handles[i], GL_TEXTURE_WRAP_S, GL_REPEAT);
		glTextureParameteri(handles[i], GL_TEXTURE_WRAP_T, GL_REPEAT);
		glTextureParameteri(handles[i], GL_TEXTURE_MIN_FILTER, GL_LINEAR);
		glTextureParameteri(handles[i], GL_TEXTURE_MAG_FILTER, GL_LINEAR);

		glTextureStorage3D(handles[i], TEXTURE_UNITS, GL_RGB8, width, width, textureCount[i]);

		width <<= 1;

		std::sprintf(uniformName, "textures[%d]", i);
		uniforms[i] = glGetUniformLocation(shaderProgram, uniformName);
	}
}

void TextureManager::bind() {
	for (mstd::U32 i = 0; i < TEXTURE_UNITS; ++i) {
		if (textureCount[i]) {
			glUniform1i(uniforms[i], i);
			glBindTextureUnit(i, handles[i]);
		}
	}
}

}
