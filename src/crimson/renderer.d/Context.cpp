#include <crimson/renderer.d/Context.hpp>
#include <iostream>

namespace ct {

Context::Context(const mstd::C8* title, mstd::I32 argc, const mstd::C8** argv) {
	using namespace mstd;

	glfwInit();

	glfwWindowHint(GLFW_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_VERSION_MINOR, 6);

	monitor = glfwGetPrimaryMonitor();
	mode = glfwGetVideoMode(monitor);
	window = glfwCreateWindow(mode->width, mode->height, title, monitor, nullptr);

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << errorText << "Could not initialize GLAD" << std::endl;
		glfwTerminate();
		return;
	}

	glfwSetWindowUserPointer(window, this);

	glfwSetFramebufferSizeCallback(window,
		[](GLFWwindow* window, int width, int height) {
			Context* context = static_cast<Context*>(glfwGetWindowUserPointer(window));

			context->resize(width, height);
		}
	);

	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}
}

void Context::run() {
	init();

	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		update();

		glfwSwapBuffers(window);
	}

	end();
}

Context::~Context() {
	glfwTerminate();
}

}
