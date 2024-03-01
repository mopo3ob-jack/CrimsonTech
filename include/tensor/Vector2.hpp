#ifndef HDB_CT_VECTOR2_HPP
#define HDB_CT_VECTOR2_HPP

#include "Vector.hpp"

#define operatorVector2(o)\
	constexpr Vector operator o (Vector vector) const {\
		return Vector(x o vector.x, y o vector.y);\
	}\
	constexpr Vector operator o##= (Vector vector) {\
		return Vector(x o##= vector.x, y o##= vector.y);\
	}

namespace hdb {

template <typename T>
class Vector<T, 2> {
public:
	constexpr Vector() {}

	constexpr Vector(T s) {
		x = s; y = s;
	}

	constexpr Vector(T x, T y) {
		this->x = x; this->y = y;
	}

	operatorVector2(+);
	operatorVector2(-);
	operatorVector2(*);
	operatorVector2(/);

	constexpr Vector operator-() const {
		return Vector(-x, -y);
	}

	constexpr Vector operator*(T s) const {
		return Vector(x * s, y * s);
	}

	constexpr Vector operator/(T s) const {
		T is = 1.0f / s;
		return Vector(x * is, y * is);
	}

	constexpr Vector normal() const {
		T p = (T)1 / sqrt(x * x + y * y);

		Vector ret = *this;

		ret *= p;

		return ret;
	}

	constexpr Vector normalize() {
		T p = (T)1 / sqrt(x * x + y * y);

		*this *= p;

		return *this;
	}

	static constexpr Vector normalize(Vector vector) {
		T p = (T)1 / sqrt(vector.x * vector.x + vector.y * vector.y);

		return vector * p;
	}

	constexpr T dot(Vector vector) const {
		return x * vector.x + y * vector.y;
	}

	static constexpr T dot(Vector a, Vector b) {
		return a.x * b.x + a.y * b.y;
	}

	constexpr T magnitude() const {
		return sqrt(x * x + y * y);
	}

	static constexpr const unsigned int getDimension() { return 2; }

	union {
		T data[2];
		struct { T x, y; };
		struct { T u, v; };
		struct { T a, b; };
	};
};

}

#endif