#ifndef CT_TRANSFORM_HPP
#define CT_TRANSFORM_HPP

#include <mstd/geometry>

namespace ct {

struct Transform {
	mstd::Vector3f position;
	mstd::Vector3f velocity;
	mstd::Vector3f acceleration;
};

}

#endif
