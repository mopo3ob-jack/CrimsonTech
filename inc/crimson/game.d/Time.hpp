#ifndef CT_TIME_HPP
#define CT_TIME_HPP

#include <mstd/geometry>

namespace ct {

class Time {
public:
	static void update();

	static mstd::F64 time;
	static mstd::F32 deltaTime;
};

}

#endif
