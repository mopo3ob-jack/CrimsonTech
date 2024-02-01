#ifndef HDB_CT_SWIZZLE3_HPP
#define HDB_CT_SWIZZLE3_HPP

namespace hdb {

namespace ct {

template <typename T, unsigned int x, unsigned int y, unsigned int z>
class Swizzle3 {
public:
	constexpr T operator=(T vector) {
		return T(c[x] = vector.x, c[y] = vector.y, c[z] = vector.z);
	}

	constexpr operator T() {
		return T(c[x], c[y], c[z]);
	}

	float c[3];
};

}

}

#endif