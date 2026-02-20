#ifndef CT_AMBIENTMAP_HPP
#define CT_AMBIENTMAP_HPP

#include <glad/glad.h>
#include <mstd/geometry>

namespace ct {

class AmbientMap {
public:
	AmbientMap() {}

	AmbientMap(mstd::Vector3<mstd::U32> dimensions, GLuint shaderProgram);

	AmbientMap(const AmbientMap& shader) = delete;
	AmbientMap& operator=(const AmbientMap& shader) = delete;

	AmbientMap(AmbientMap&& shader) = default;

	AmbientMap& operator=(AmbientMap&& shader) {
		this->~AmbientMap();

		return *this;
	}

	void bind();

private:
	GLuint uniform;
	GLuint handle;
};

}

#endif
