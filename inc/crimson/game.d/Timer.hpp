#ifndef CT_TIMER_HPP
#define CT_TIMER_HPP

#include <crimson/game.d/Time.hpp>
#include <GLFW/glfw3.h>

namespace ct {

class Timer {
public:
	Timer() {
		reset();
	}

	void reset() {
		accumulated = 0;
		t = 0.0;
	}

	void restart() {
		accumulated = 0;
		t = glfwGetTime();
	}

	void start() {
		if (t == 0.0) {
			t = glfwGetTime();
		}
	}

	void stop() {
		if (t != 0.0) {
			accumulated += glfwGetTime() - t;
			t = 0.0;
		}
	}

	mstd::F64 get() const {
		if (t == 0.0) {
			return accumulated;
		} else {
			return accumulated + glfwGetTime() - t;
		}
	}

private:
	mstd::F64 t;
	mstd::F64 accumulated;
};

}

#endif
