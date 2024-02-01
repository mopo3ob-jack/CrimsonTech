#ifndef HDB_CT_VECTOR_HPP
#define HDB_CT_VECTOR_HPP

#include <immintrin.h>
#include <math.h>
#include <initializer_list>
#include <concepts>

#define operatorVector(o)\
	constexpr Vector<rows> operator o (Vector<rows> vector) const {\
		Vector ret;\
		for (unsigned int i = 0; i < rows; ++i) { ret.data[i] = this->data[i] o vector.data[i]; }\
		return ret;\
	}\
	constexpr Vector<rows> operator o##= (Vector<rows> vector) {\
		for (unsigned int i = 0; i < rows; ++i) { this->data[i] o##= vector.data[i]; }\
	}

namespace hdb {

template <unsigned int rows>
class Vector {
public:
	constexpr Vector() {}

	constexpr Vector(float s) {
		for (unsigned int i = 0; i < rows; ++i) { this->data[i] = s; }
	}

	operatorVector(+);
	operatorVector(-);
	operatorVector(*);
	operatorVector(/);

	constexpr Vector operator-() const {
		Vector ret;
		for (unsigned int i = 0; i < rows; ++i) { ret[i] = -this->data[i]; }
		return ret;
	}

	constexpr Vector operator*(float s) const {
		Vector ret;
		for (unsigned int i = 0; i < rows; ++i) { ret[i] = this->data[i] * s; }
		return ret;
	}

	constexpr Vector operator/(float s) const {
		float is = 1.0f / s;
		Vector ret;
		for (unsigned int i = 0; i < rows; ++i) { ret[i] = this->data[i] * is; }
		return ret;
	}

	constexpr Vector normalize() {
		float p = 0.0f;
		for (unsigned int i = 0; i < rows; ++i) { p += this->data[i] * this->data[i]; }

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
		float p = 0.0f;
		for (unsigned int i = 0; i < rows; ++i) { p += vector.data[i] * vector.data[i]; }

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
		float p = 0.0f;
		for (unsigned int i = 0; i < rows; ++i) { p += this->data[i] * this->data[i]; }

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
		float ret;
		for (unsigned int i = 0; i < rows; ++i) { ret += this->data[i] * vector.data[i]; }
		return ret;
	}

	static constexpr float dot(Vector a, Vector b) {
		float ret;
		for (unsigned int i = 0; i < rows; ++i) { ret += a.data[i] * b.data[i]; }
		return ret;
	}

	constexpr float magnitude() const {
		float ret;
		for (unsigned int i = 0; i < rows; ++i) { ret += this->data[i] * this->data[i]; }
		ret = sqrt(ret);
		return ret;
	}

	static constexpr const unsigned int getDimension() { return rows; }

	float data[rows];
};

}

#endif