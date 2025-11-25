#ifndef CT_FREECAMERA_HPP
#define CT_FREECAMERA_HPP

#include <crimson/game.d/Camera.hpp>

namespace ct {

class FreeCamera : public Camera {
public:
	void update();

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
