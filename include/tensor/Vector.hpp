#ifndef HDB_CT_VECTOR_HPP
#define HDB_CT_VECTOR_HPP

#include <immintrin.h>
#include <math.h>
#include <initializer_list>
#include <concepts>

#define operatorVector(T, o)\
	constexpr Vector<T, rows> operator o (Vector<T, rows> vector) const {\
		Vector ret;\
		for (unsigned int i = 0; i < rows; ++i) { ret.data[i] = this->data[i] o vector.data[i]; }\
		return ret;\
	}\
	constexpr Vector<T, rows> operator o##= (Vector<T, rows> vector) {\
		for (unsigned int i = 0; i < rows; ++i) { this->data[i] o##= vector.data[i]; }\
	}

namespace hdb {

template <typename T, unsigned int rows>
class Vector {
public:
	constexpr Vector() {}

	constexpr Vector(T s) {
		for (unsigned int i = 0; i < rows; ++i) { this->data[i] = s; }
	}

	operatorVector(T, +);
	operatorVector(T, -);
	operatorVector(T, *);
	operatorVector(T, /);

	constexpr Vector operator-() const {
		Vector ret;
		for (unsigned int i = 0; i < rows; ++i) { ret[i] = -this->data[i]; }
		return ret;
	}

	constexpr Vector operator*(T s) const {
		Vector ret;
		for (unsigned int i = 0; i < rows; ++i) { ret[i] = this->data[i] * s; }
		return ret;
	}

	constexpr Vector operator/(T s) const {
		float is = 1.0f / s;
		Vector ret;
		for (unsigned int i = 0; i < rows; ++i) { ret[i] = this->data[i] * is; }
		return ret;
	}

	static constexpr const unsigned int getDimension() { return rows; }

	T data[rows];
};

typedef Vector<float, 2> Vector2f;
typedef Vector<float, 3> Vector3f;
typedef Vector<float, 4> Vector4f;

typedef Vector<double, 2> Vector2d;
typedef Vector<double, 3> Vector3d;
typedef Vector<double, 4> Vector4d;

typedef Vector<char, 2> Vector2c;
typedef Vector<char, 3> Vector3c;
typedef Vector<char, 4> Vector4c;

typedef Vector<unsigned char, 2> Vector2uc;
typedef Vector<unsigned char, 3> Vector3uc;
typedef Vector<unsigned char, 4> Vector4uc;

typedef Vector<short, 2> Vector2s;
typedef Vector<short, 3> Vector3s;
typedef Vector<short, 4> Vector4s;

typedef Vector<ushort, 2> Vector2us;
typedef Vector<ushort, 3> Vector3us;
typedef Vector<ushort, 4> Vector4us;

typedef Vector<int, 2> Vector2i;
typedef Vector<int, 3> Vector3i;
typedef Vector<int, 4> Vector4i;

typedef Vector<uint, 2> Vector2ui;
typedef Vector<uint, 3> Vector3ui;
typedef Vector<uint, 4> Vector4ui;

typedef Vector<long, 2> Vector2l;
typedef Vector<long, 3> Vector3l;
typedef Vector<long, 4> Vector4l;

typedef Vector<ulong, 2> Vector2ul;
typedef Vector<ulong, 3> Vector3ul;
typedef Vector<ulong, 4> Vector4ul;

}

#endif