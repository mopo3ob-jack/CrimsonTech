#ifndef CT_FLYCAMERA_HPP
#define CT_FLYCAMERA_HPP

#include <crimson/game.d/Camera.hpp>
#include <crimson/file.d/BSP.hpp>

namespace ct {

class FlyCamera : public Camera {
public:
	void update(BSP& bsp);

	enum : mstd::U32 {
		FORWARD = 1,
		BACKWARD = 2,
		RIGHT = 4,
		LEFT = 8,
		UP = 16,
		DOWN = 32,
	};

	mstd::U32 keyMask;
};

}

#endif