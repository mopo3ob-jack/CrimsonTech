#ifndef HDB_CT_VERTEX_HPP
#define HDB_CT_VERTEX_HPP

#include "../tensor/tensor.hpp"

namespace hdb {

namespace ct {

struct Vertex {
	Vector3 position;
	Vector2 texCoords;
	unsigned char color[4];
	Vector3 normal;
	Vector3 tangent;
	Vector3 bitangent;
};

}

}

#endif