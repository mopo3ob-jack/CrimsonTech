#ifndef CT_CONTEXT_HPP
#define CT_CONTEXT_HPP

#include <mstd/misc>
#include <crimson/renderer.d/gl.hpp>
#define GLFW_INCLUDE_NONE
#include <GLFW/glfw3.h>

namespace ct {

class Context {
public:
	Context(const mstd::C8* title, mstd::I32 argc = 0, const mstd::C8** argv = nullptr);

	~Context();

	void run();

	virtual void init() {}
	virtual void update() {}
	virtual void end() {}
	virtual void resize(mstd::I32 width, mstd::I32 height) {
		glViewport(0, 0, width, height);
	}

	GLFWwindow* window;
	GLFWmonitor* monitor;
	const GLFWvidmode* mode;
};

}

#endif
