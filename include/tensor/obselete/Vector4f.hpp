#ifndef HDB_CT_VECTOR4F_HPP
#define HDB_CT_VECTOR4F_HPP

#include "Vector.hpp"

#define operatorVector4(o)\
	constexpr Vector operator o (Vector vector) const {\
		return Vector(x o vector.x, y o vector.y, z o vector.z, w o vector.w);\
	}\
	constexpr Vector operator o##= (Vector vector) {\
		return Vector(x o##= vector.x, y o##= vector.y, z o##= vector.z, w o##= vector.w);\
	}

namespace hdb {

template <>
class Vector<float, 4> {
public:
	constexpr Vector() {}

	constexpr Vector(float s) {
		x = s; y = s; z = s; w = s;
	}

	constexpr Vector(float x, float y, float z, float w) {
		this->x = x; this->y = y; this->z = z; this->w = w;
	}

	operatorVector4(+);
	operatorVector4(-);
	operatorVector4(*);
	operatorVector4(/);

	constexpr Vector operator-() const {
		return Vector(-x, -y, -z, -w);
	}

	constexpr Vector operator*(float s) const {
		return Vector(x * s, y * s, z * s, w * s);
	}

	constexpr Vector operator/(float s) const {
		float is = 1.0f / s;
		return Vector(x * is, y * is, z * is, w * is);
	}

	constexpr Vector normalize() {
		float p = x * x + y * y + z * z + w * w;

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
		float p = vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w;

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
		float p = x * x + y * y + z * z + w * w;

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
		return x * vector.x + y * vector.y + z * vector.z + w * vector.w;
	}

	static constexpr float dot(Vector a, Vector b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	constexpr float magnitude() const {
		return sqrt(x * x + y * y + z * z + w * w);
	}

	static constexpr const unsigned int getDimension() { return 4; }

	union {
		float data[4];
		struct { float x, y, z, w; };
		struct { float r, g, b, a; };
		struct { float s, bxy, byz, bzx; }; //For rotors
	};
};

}

#endif