#ifndef CT_LIGHT_HPP
#define CT_LIGHT_HPP

#include <mstd/geometry>

namespace ct {

struct Light {
	mstd::Vector3f position, color;
	mstd::F32 diffuse, specular;
};

}

#endif
