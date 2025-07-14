#include <crimson/game.d/Time.hpp>
#include <GLFW/glfw3.h>

namespace ct {

mstd::F64 Time::time = 0.0f;
mstd::F32 Time::deltaTime = 0.0f;

void Time::update() {
	mstd::F64 newTime = glfwGetTime();
	deltaTime = newTime - time;

	time = newTime;
}

}
