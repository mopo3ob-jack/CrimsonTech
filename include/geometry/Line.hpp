#ifndef HDB_CT_LINE_HPP
#define HDB_CT_LINE_HPP

#include "../tensor/tensor.hpp"

namespace hdb {

namespace ct {

class Line {
public:
	constexpr Vector3 intersection(Line line) {
		return Vector3();
	}

	Vector3 a, b;
};

}

}

#endif