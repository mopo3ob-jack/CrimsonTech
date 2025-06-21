#ifndef CT_BSP_HPP
#define CT_BSP_HPP

#include <crimson/renderer.d/VertexArray.hpp>
#include <crimson/types.d/Plane.hpp>
#include <string>
#include <mstd/memory>

namespace ct {

class BSP {
public:
	BSP() {}

	~BSP() {}

	void build(const std::string& path);

	VertexArray vertexArray;
private:
	mstd::Tree<Plane, 2> rootSplit;
};

}

#endif
