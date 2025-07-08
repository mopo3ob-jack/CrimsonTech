#ifndef CT_BSP_HPP
#define CT_BSP_HPP

#include <crimson/renderer.d/VertexArray.hpp>
#include <crimson/types.d/Plane.hpp>
#include <string>
#include <mstd/memory>

namespace ct {

class BSP {
public:
	using Node = mstd::Tree<Plane, 2>;
	using VectorParam = const mstd::Vector3f&;

	BSP() : arena(1L << 18) {}

	~BSP() {}

	void build(const std::string& path);

	mstd::Bool colliding(const mstd::Vector3f& point) const;

	struct Intersection {
		mstd::Vector3f point;
		Plane plane;
	};

	mstd::Bool intersect(
		const mstd::Vector3f& from,
		const mstd::Vector3f& to,
		Intersection& result
	) const;

	void print() const;

	VertexArray vertexArray;
	mstd::U32 indexCount;
private:
	mstd::Arena arena;
	Node rootSplit;

	mstd::Bool intersect(
		const mstd::Vector3f& from,
		const mstd::Vector3f& to,
		Intersection& result,
		const Node& node
	) const;
};

}

#endif
