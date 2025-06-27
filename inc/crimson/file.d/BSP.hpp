#ifndef CT_BSP_HPP
#define CT_BSP_HPP

#include <crimson/renderer.d/VertexArray.hpp>
#include <crimson/types.d/Plane.hpp>
#include <string>
#include <mstd/memory>

namespace ct {

class BSP {
public:
	BSP() : arena(1L << 18) {}

	~BSP() {}

	void build(const std::string& path);

	mstd::Bool colliding(const mstd::Vector3f& point) const;

	void print() const;

	VertexArray vertexArray;
	mstd::U32 indexCount;
private:
	mstd::Arena arena;
	mstd::Tree<Plane, 2> rootSplit;
};

}

#endif
