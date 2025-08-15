#ifndef CT_POINTLIGHT_HPP
#define CT_POINTLIGHT_HPP

#include <crimson/renderer.d/Light.hpp>

namespace ct {

struct PointLight : public Light {
	mstd::F32 linear, quadratic;
};

}

#endif
