#ifndef CT_CONTEXT_HPP
#define CT_CONTEXT_HPP

#include <mstd/misc>
#include <GLFW/glfw3.h>

namespace ct {

class Context {
public:
	Context(mstd::C8* title, mstd::I32 argc = 0, char** argv = nullptr);

	~Context();

	void run();

	virtual void init() {}
	virtual void update() {}
	virtual void end() {}
	virtual void resize(mstd::I32 width, mstd::I32 height);

	GLFWwindow* window;
	GLFWmonitor* monitor;
	const GLFWvidmode* mode;
};

}

#endif
