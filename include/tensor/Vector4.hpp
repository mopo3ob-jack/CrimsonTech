#ifndef HDB_CT_VECTOR4_HPP
#define HDB_CT_VECTOR4_HPP

#include "Vector.hpp"

#define operatorVector4(o)\
	constexpr Vector operator o (Vector vector) const {\
		return Vector(x o vector.x, y o vector.y, z o vector.z, w o vector.w);\
	}\
	constexpr Vector operator o##= (Vector vector) {\
		return Vector(x o##= vector.x, y o##= vector.y, z o##= vector.z, w o##= vector.w);\
	}

namespace hdb {

template <typename T>
class Vector<T, 4> {
public:
	constexpr Vector() {}

	constexpr Vector(T s) {
		x = s; y = s; z = s; w = s;
	}

	constexpr Vector(T x, T y, T z, T w) {
		this->x = x; this->y = y; this->z = z; this->w = w;
	}

	operatorVector4(+);
	operatorVector4(-);
	operatorVector4(*);
	operatorVector4(/);

	constexpr Vector operator-() const {
		return Vector(-x, -y, -z, -w);
	}

	constexpr Vector operator*(T s) const {
		return Vector(x * s, y * s, z * s, w * s);
	}

	constexpr Vector operator/(T s) const {
		T is = 1.0f / s;
		return Vector(x * is, y * is, z * is, w * is);
	}

	constexpr Vector normal() const {
		T p = (T)1 / sqrt(x * x + y * y + z * z + w * w);

		Vector ret = *this;

		ret *= p;

		return ret;
	}

	constexpr Vector normalize() {
		T p = (T)1 / sqrt(x * x + y * y + z * z + w * w);

		*this *= p;

		return *this;
	}

	static constexpr Vector normalize(Vector vector) {
		T p = (T)1 / sqrt(vector.x * vector.x + vector.y * vector.y + vector.z * vector.z + vector.w * vector.w);

		return vector * p;
	}

	constexpr T dot(Vector vector) const {
		return x * vector.x + y * vector.y + z * vector.z + w * vector.w;
	}

	static constexpr T dot(Vector a, Vector b) {
		return a.x * b.x + a.y * b.y + a.z * b.z + a.w * b.w;
	}

	constexpr T magnitude() const {
		return sqrt(x * x + y * y + z * z + w * w);
	}

	static constexpr const unsigned int getDimension() { return 4; }

	union {
		T data[4];
		struct { T x, y, z, w; };
		struct { T r, g, b, a; };
		struct { T xy, yz, zx; };
	};
};

}

#endif