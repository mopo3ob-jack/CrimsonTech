#ifndef HDB_CT_BACKENDINIT_HPP
#define HDB_CT_BACKENDINIT_HPP

#include "../utils/utils.hpp"
#include "../tensor/tensor.hpp"

#include "../../glad/include/glad/glad.h"
#include <GLFW/glfw3.h>

#include <concepts>
#include <mutex>

namespace hdb {

namespace ct {

namespace Context {
	extern GLFWwindow* window;
	extern GLFWmonitor* monitor;
	extern const GLFWvidmode* mode;
			
	struct Time {
		static double time;
		static float deltaTime;
		static float fps;
	};

	struct Input {
		static Vector2f mousePosition;
		static Vector2f mouseDelta;
		static double mouseX, mouseY;
	};
}

extern std::mutex contextLock;

static void initCrimson(unsigned int width, unsigned int height, const char* title, GLFWmonitor* monitor) {
	using namespace Context;
	
	glfwWindowHint(GLFW_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_VERSION_MINOR, 5);

	window = glfwCreateWindow(width, height, title, monitor, nullptr);

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		hdb::error("Could not initialize GLAD\n");
		glfwTerminate();

		exit(1);
	}

	glGetTextureHandleARB = (PFNGLGETTEXTUREHANDLEARBPROC)glfwGetProcAddress("glGetTextureHandleARB");
	glMakeTextureHandleResidentARB = (PFNGLMAKETEXTUREHANDLERESIDENTARBPROC)glfwGetProcAddress("glMakeTextureHandleResidentARB");
	glMakeTextureHandleNonResidentARB = (PFNGLMAKETEXTUREHANDLENONRESIDENTARBPROC)glfwGetProcAddress("glMakeTextureHandleNonResidentARB");
	

	bool triggerError = false;
	if (!glGetTextureHandleARB) {
		hdb::error("glGetTextureHandleARB is equal to NULL\n");
		triggerError = true;
	}
	if (!glMakeTextureHandleResidentARB) {
		hdb::error("glMakeTextureHandleResidentARB is equal to NULL\n");
		triggerError = true;
	}
	if (!glMakeTextureHandleNonResidentARB) {
		hdb::error("glMakeTextureHandleNonResidentARB is equal to NULL\n");
		triggerError = true;
	}
	if (triggerError)
		hdb::error("Check if you have a graphics card that supports OpenGL 4.0 or higher!\n\tAnd can use GL_ARB_BINDLESS_TEXTURE\n\t(Preferably OpenGL 4.5)\n");
}

static void terminateCrimson() {
	glfwTerminate();
}

template <typename T>
static void runCrimsonApp() {
	T app;
	app.run();
}

}

}

#endif