#ifndef HDB_CT_PLANE_HPP
#define HDB_CT_PLANE_HPP

#include "../tensor/tensor.hpp"

namespace hdb {

namespace ct {

class Plane {
public:
	constexpr Plane() {}

	constexpr Plane(const Plane& plane) {
		this->normal = plane.normal;
		this->distance = plane.distance;
	}

	constexpr Plane(Vector3 normal, float distance) {
		this->normal = normal; this->distance = distance;
	}

	constexpr Plane(Vector3* vertices) {
		Vector3 a = vertices[1] - vertices[0];
		Vector3 b = vertices[2] - vertices[0];
		normal = a.cross(b);
		normal.normalize();

		distance = normal.dot(vertices[0]);
	}

	constexpr float distanceToPlane(Vector3 point) const {
		return point.dot(normal) - distance;
	}

	constexpr Vector3 intersection(Vector3 a, Vector3 b) const {
		Vector3 direction = b - a;

		float t = (distance - Vector3::dot(normal, a)) / Vector3::dot(normal, direction);

		return a + direction * t;
	}

	Vector3 normal;
	float distance;
};

}

}

#endif