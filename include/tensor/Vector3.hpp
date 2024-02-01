#ifndef HDB_CT_VECTOR3_HPP
#define HDB_CT_VECTOR3_HPP

#include "Vector.hpp"

#include <cmath>
#include <immintrin.h>

#define operatorVector3(o)\
	constexpr Vector operator o (Vector vector) const {\
		return Vector(x o vector.x, y o vector.y, z o vector.z);\
	}\
	constexpr Vector operator o##= (Vector vector) {\
		return Vector(x o##= vector.x, y o##= vector.y, z o##= vector.z);\
	}

namespace hdb {

template <>
class Vector<3> {
public:
	constexpr Vector() {}

	constexpr Vector(float s) {
		x = s; y = s; z = s;
	}

	constexpr Vector(float x, float y, float z) {
		this->x = x; this->y = y; this->z = z;
	}

	operatorVector3(+);
	operatorVector3(-);
	operatorVector3(*);
	operatorVector3(/);

	constexpr Vector operator-() const {
		return Vector(-x, -y, -z);
	}

	constexpr Vector operator*(float s) const {
		return Vector(x * s, y * s, z * s);
	}

	constexpr Vector operator/(float s) const {
		float is = 1.0f / s;
		return Vector(x * is, y * is, z * is);
	}

	constexpr Vector normalize() {
		float p = x * x + y * y + z * z;

		if (std::is_constant_evaluated()) {
			p = 1.0f / sqrt(p);
		} else {
			__m128 mag = _mm_load_ss(&p);
			mag = _mm_rsqrt_ss(mag);
			_mm_store_ss(&p, mag);
		}

		this->operator*=(p);
		return *this;
	}

	static constexpr Vector normalize(Vector vector) {
		float p = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z;

		if (std::is_constant_evaluated()) {
			p = 1.0f / sqrt(p);
		} else {
			__m128 mag = _mm_load_ss(&p);
			mag = _mm_rsqrt_ss(mag);
			_mm_store_ss(&p, mag);
		}

		return vector * p;
	}

	constexpr Vector normal() const {
		float p = x * x + y * y + z * z;

		if (std::is_constant_evaluated()) {
			p = 1.0f / sqrt(p);
		} else {
			__m128 mag = _mm_load_ss(&p);
			mag = _mm_rsqrt_ss(mag);
			_mm_store_ss(&p, mag);
		}

		Vector ret = *this;

		ret *= p;

		return ret;
	}

	constexpr float dot(Vector vector) const {
		return x * vector.x + y * vector.y + z * vector.z;
	}

	static constexpr float dot(Vector a, Vector b) {
		return a.x * b.x + a.y * b.y + a.z * b.z;
	}

	constexpr float magnitude() const {
		return sqrt(x * x + y * y + z * z);
	}
	
	constexpr Vector cross(Vector vector) const {
		return Vector(y * vector.z - z * vector.y, z * vector.x - x * vector.z, x * vector.y - y * vector.x);
	}

	static constexpr Vector cross(Vector a, Vector b) {
		return Vector(a.y * b.z - a.z * b.y, a.z * b.x - a.x * b.z, a.x * b.y - a.y * b.x);
	}

	static constexpr const unsigned int getDimension() { return 3; }

	union {
		float data[3];
		struct { float x, y, z; };
		struct { float r, g, b; };
	};
};

typedef Vector<3> Vector3;

}

#endif