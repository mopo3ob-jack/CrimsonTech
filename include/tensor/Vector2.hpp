#ifndef HDB_CT_VECTOR2_HPP
#define HDB_CT_VECTOR2_HPP

#include "Vector.hpp"

#include <cmath>
#include <immintrin.h>

#define operatorVector2(o)\
	constexpr Vector operator o (Vector vector) const {\
		return Vector(x o vector.x, y o vector.y);\
	}\
	constexpr Vector operator o##= (Vector vector) {\
		return Vector(x o##= vector.x, y o##= vector.y);\
	}

namespace hdb {

template <>
class Vector<2> {
public:
	constexpr Vector() {}

	constexpr Vector(float s) {
		x = s; y = s;
	}

	constexpr Vector(float x, float y) {
		this->x = x; this->y = y;
	}

	operatorVector2(+);
	operatorVector2(-);
	operatorVector2(*);
	operatorVector2(/);

	constexpr Vector operator-() const {
		return Vector(-x, -y);
	}

	constexpr Vector operator*(float s) const {
		return Vector(x * s, y * s);
	}

	constexpr Vector operator/(float s) const {
		float is = 1.0f / s;
		return Vector(x * is, y * is);
	}

	constexpr Vector normalize() {
		float p = x * x + y * y;

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
		float p = vector.x * vector.x + vector.y * vector.y;

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
		float p = x * x + y * y;

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
		return x * vector.x + y * vector.y;
	}

	static constexpr float dot(Vector a, Vector b) {
		return a.x * b.x + a.y * b.y;
	}

	constexpr float magnitude() const {
		return sqrt(x * x + y * y);
	}

	static constexpr const unsigned int getDimension() { return 2; }

	union {
		float data[2];
		struct { float x, y; };
		struct { float u, v; };
		struct { float a, b; };
	};
};

typedef Vector<2> Vector2;

}

#endif