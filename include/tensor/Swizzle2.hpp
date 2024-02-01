#ifndef HDB_CT_SWIZZLE2_HPP
#define HDB_CT_SWIZZLE2_HPP

namespace hdb {

namespace ct {

template <typename T, unsigned int x, unsigned int y>
class Swizzle2 {
public:
	constexpr T operator=(T vector) {
		return T(c[x] = vector.x, c[y] = vector.y);
	}

	constexpr operator T() {
		return T(c[x], c[y]);
	}

	float c[2];
};

}

}

#endif