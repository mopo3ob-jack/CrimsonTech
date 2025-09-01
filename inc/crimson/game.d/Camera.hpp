#ifndef CT_CAMERA_HPP
#define CT_CAMERA_HPP

#include <mstd/geometry>

namespace ct {

class Camera {
public:
	constexpr void update() {
		using namespace mstd;

		viewMatrix = 
			rotateY(-angle.y)
			* rotateX(-angle.x)
			* rotateZ(-angle.z)
			* translate(-position)
		;
	}

	mstd::Vector3f position;
	mstd::Vector3f angle;
	mstd::Matrix4f viewMatrix;
};

}

#endif
