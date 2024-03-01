#ifndef HDB_CT_VERTEX_HPP
#define HDB_CT_VERTEX_HPP

#include "../tensor/tensor.hpp"

namespace hdb {

namespace ct {

struct Vertex {
	Vector3f position;
	Vector2f texCoords;
	Vector3f normal;
	Vector3f tangent;
	Vector3f bitangent;
	unsigned int textureID;
	unsigned int transformID;
};

}

}

#endif