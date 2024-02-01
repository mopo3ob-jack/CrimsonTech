#ifndef HDB_CT_CONPLANE_HPP
#define HDB_CT_CONPLANE_HPP

#include "../tensor/tensor.hpp"
#include "Plane.hpp"

namespace hdb {

namespace ct {

//Conservative Plane uses less memory at the cost of having slower computations
class ConPlane {
public:
	Vector3 direction;
};

}

}

#endif