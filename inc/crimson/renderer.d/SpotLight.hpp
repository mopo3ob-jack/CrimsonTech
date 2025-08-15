#ifndef CT_SPOTLIGHT_HPP
#define CT_SPOTLIGHT_HPP

#include <crimson/renderer.d/Light.hpp>

namespace ct {

struct SpotLight : public Light {
	mstd::Vector3f direction;
	mstd::F32 linear, quadratic;
	mstd::F32 innerRadius, outerRadius;
};

}

#endif
