#ifndef CT_PLAYER_HPP
#define CT_PLAYER_HPP

#include <crimson/file.d/BSP.hpp>
#include <crimson/game.d/Time.hpp>
#include <crimson/game.d/Camera.hpp>

namespace ct {

class Player : public Camera {
public:
	void update(BSP& bsp);

	enum : mstd::U32 {
		FORWARD = 1,
		BACKWARD = 2,
		RIGHT = 4,
		LEFT = 8,
		JUMP = 16,
	};

	mstd::U32 keyMask;

	mstd::Vector3f velocity;
	mstd::U32 airFrames;
};

}

#endif
