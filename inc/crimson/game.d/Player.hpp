#ifndef CT_PLAYER_HPP
#define CT_PLAYER_HPP

#include <crimson/file.d/BSP.hpp>
#include <crimson/game.d/Time.hpp>

namespace ct {

class Player {
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

	mstd::Vector3f angle;
	mstd::Vector3f position, velocity;
	mstd::U32 airFrames;
};

}

#endif
