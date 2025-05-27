#ifndef CT_PLANE_HPP
#define CT_PLANE_HPP

#include <crimson/types.d/Face.hpp>
#include <mstd/memory>

namespace ct {

class Plane {
public:
	Plane() {}

	Plane(mstd::Vector3f normal, mstd::F32 d) : normal(normal), d(d) {}

	Plane(const Face& f) {
		normal = f.normal();
		d = mstd::dot(normal, f.vertices[0]);
	}

	mstd::F32 distance(const mstd::Vector3f& v) const {
		return dot(normal, v) - d;
	}

	mstd::Size split(const Face& f, Face*& result, mstd::Arena& arena) {
		using namespace mstd;

		Size splits = 0;

		Bool inFront;
		Vector3f& a = f.vertices[f.count - 1];
		for (Size i = 0; i < f.count; ++i) {
			Vector3f& b = f.vertices[i];

			Vector3f l = b - a;

			F32 t = (d - dot(normal, a)) / dot(normal, l);



			a = f.vertices[i];
		}
	}

public:
	mstd::Vector3f normal;
	mstd::F32 d;
};

}

#endif
