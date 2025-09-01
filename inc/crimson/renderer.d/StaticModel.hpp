#ifndef CT_STATICMODEL_HPP
#define CT_STATICMODEL_HPP

#include <string>
#include <crimson/renderer.d/VertexArray.hpp>

namespace ct {

class StaticModel {
public:
	StaticModel() {}

	StaticModel(const std::string& path);

	VertexArray vertexArray;
};

}

#endif
