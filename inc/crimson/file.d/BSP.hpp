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

	BSP() {}

	~BSP() {}

	void build(const std::string& path, mstd::Arena& arena);

	mstd::Bool colliding(const mstd::Vector3f& point) const;

	struct Trace {
		mstd::Vector3f point;
		Plane plane;
	};

	mstd::Bool clip(
		const mstd::Vector3f& from,
		const mstd::Vector3f& to,
		Trace& result
	) const;

	void print() const;

	VertexArray vertexArray;
	mstd::U32 indexCount;

private:
	Node rootSplit;

	mstd::Bool isSolid(const mstd::Vector3f& point, const Node* node) const;

	mstd::Bool clip(
		const mstd::Vector3f& from,
		const mstd::Vector3f& to,
		Trace& result,
		const Node& node
	) const;
};

}

#endif
