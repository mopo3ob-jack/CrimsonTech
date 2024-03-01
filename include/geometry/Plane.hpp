#ifndef HDB_CT_PLANE_HPP
#define HDB_CT_PLANE_HPP

#include "../tensor/tensor.hpp"

namespace hdb {

namespace ct {

class Plane {
public:
	constexpr Plane() {}

	constexpr Plane(const Plane& plane) {
		this->normal = plane.normal; sizeof(Vertex);
		this->distance = plane.distance;
	}

	constexpr Plane(Vector3f normal, float distance) {
		this->normal = normal; this->distance = distance;
	}

	constexpr Plane(Vector3f* vertices) {
		Vector3f a = vertices[1] - vertices[0];
		Vector3f b = vertices[2] - vertices[0];
		normal = a.cross(b);
		normal.normalize();

		distance = normal.dot(vertices[0]);
	}

	constexpr float distanceToPlane(Vector3f point) const {
		return point.dot(normal) - distance;
	}

	constexpr Vector3f intersection(Vector3f a, Vector3f b) const {
		Vector3f direction = b - a;

		float t = (distance - Vector3f::dot(normal, a)) / Vector3f::dot(normal, direction);

		return a + direction * t;
	}

	Vector3f normal;
	float distance;
};

}

}

#endif