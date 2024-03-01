#ifndef HDB_CT_LINE_HPP
#define HDB_CT_LINE_HPP

#include "../tensor/tensor.hpp"

namespace hdb {

namespace ct {

class Line {
public:
	constexpr Vector3f intersection(Line line) {
		return Vector3f();
	}

	Vector3f a, b;
};

}

}

#endif