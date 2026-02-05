#ifndef CT_TEXTUREMANAGER_HPP
#define CT_TEXTUREMANAGER_HPP

#include <glad/glad.h>
#include <mstd/geometry>
#include <iostream>

namespace ct {

class TextureManager {
public:
	static constexpr mstd::Size MIN_TEXTURE_UNIT = 3;
	static constexpr mstd::Size MAX_TEXTURE_UNIT = 12;
	static constexpr mstd::Size MIN_TEXTURE_WIDTH = 1 << MIN_TEXTURE_UNIT;
	static constexpr mstd::Size MAX_TEXTURE_WIDTH = 1 << MAX_TEXTURE_UNIT;
	static constexpr mstd::Size TEXTURE_UNITS = MAX_TEXTURE_UNIT - MIN_TEXTURE_UNIT + 1;

	TextureManager() : handles(0) {}

	TextureManager(mstd::U32 textureCount[TEXTURE_UNITS], GLuint shader);

	TextureManager(const TextureManager&) = delete;
	TextureManager& operator=(const TextureManager&) = delete;

	TextureManager(TextureManager&& tm) = default;

	TextureManager& operator=(TextureManager&& tm) {
		this->~TextureManager();

		std::copy(tm.uniforms, tm.uniforms + TEXTURE_UNITS, uniforms);
		std::copy(tm.handles, tm.handles + TEXTURE_UNITS, handles);
		std::copy(tm.textureCount, tm.textureCount + TEXTURE_UNITS, textureCount);

		std::fill(tm.handles, tm.handles + TEXTURE_UNITS, 0);
		std::fill(tm.textureCount, tm.textureCount + TEXTURE_UNITS, 0);

		return *this;
	}

	~TextureManager() {
		glDeleteTextures(TEXTURE_UNITS, handles);

		std::fill(handles, handles + TEXTURE_UNITS, 0);
		std::fill(textureCount, textureCount + TEXTURE_UNITS, 0);
	}

	void write(
		mstd::U32 unit,
		mstd::U32 index,
		mstd::U8* data,
		mstd::U32 width,
		mstd::U32 level = 0,
		mstd::Vector2<mstd::U32> offset = {0, 0}
	) {
		glTextureSubImage3D(
			handles[unit], level, offset.x, offset.y, index, width, width, 1, GL_RGBA, GL_UNSIGNED_BYTE, data
		);
	}

	void bind();

private:
	GLuint uniforms[TEXTURE_UNITS];
	GLuint handles[TEXTURE_UNITS];
	mstd::U32 textureCount[TEXTURE_UNITS];
};

}

#endif
