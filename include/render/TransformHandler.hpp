#ifndef HDB_CT_TRANSFORMHANDLER_HPP
#define HDB_CT_TRANSFORMHANDLER_HPP

#include "../geometry/geometry.hpp"
#include <vector>

namespace hdb {

namespace ct {

class TransformHandler {
public:
	TransformHandler() {}

	std::vector<Transform*> transforms;
};

}

}

#endif