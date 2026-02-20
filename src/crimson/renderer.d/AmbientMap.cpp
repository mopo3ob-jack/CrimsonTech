#include <crimson/renderer.d/AmbientMap.hpp>

namespace ct {

AmbientMap::AmbientMap(mstd::Vector3<mstd::U32> dimensions, GLuint shaderProgram) {
	using namespace mstd;

	glCreateTextures(GL_TEXTURE_3D, 1, &handle);

	uniform = glGetUniformLocation(shaderProgram, "ambientMap");

	glTextureParameteri(handle, GL_TEXTURE_WRAP_S, GL_CLAMP_TO_EDGE);
	glTextureParameteri(handle, GL_TEXTURE_WRAP_T, GL_CLAMP_TO_EDGE);
	glTextureParameteri(handle, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glTextureParameteri(handle, GL_TEXTURE_MAG_FILTER, GL_LINEAR);

	glTextureStorage3D(handle, 1, GL_RGB8, dimensions.x, dimensions.y, dimensions.z);
}

}
