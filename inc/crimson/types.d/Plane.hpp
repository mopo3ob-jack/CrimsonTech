#ifndef CT_PLANE_HPP
#define CT_PLANE_HPP

#include <mstd/geometry>
#include <iostream>

namespace ct {

class Plane {
public:
	mstd::F32 distance(const mstd::Vector3f& p) const {
		return dot(p, normal) - d;
	}

	mstd::Bool intersect(const mstd::Vector3f& a, const mstd::Vector3f& b, mstd::Vector3f& p) const {
		using namespace mstd;

		Vector3f direction = b - a;

		F32 denom = dot(normal, direction);
		if (denom == 0.0f) return false;

		F32 t = (d - dot(normal, a)) / denom;
		if (t <= 0.0f || t >= 1.0f) return false;

		p = a + direction * t;

		return true;
	}

	mstd::Vector3f normal;
	mstd::F32 d;
};

}

#endif
