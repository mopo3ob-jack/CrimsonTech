#ifndef HDB_CT_APPBASE_HPP
#define HDB_CT_APPBASE_HPP

#include "ScriptHandler.hpp"
#include "Context.hpp"

#include "../utils/utils.hpp"
#include "../render/render.hpp"

#include "../../glad/include/glad/glad.h"
#include <GLFW/glfw3.h>

namespace hdb {

namespace ct {

class AppBase {
public:
	virtual void run() final {
		using namespace Context;
		
		start();

		const double invWidth = 1.0f / mode->width;
		const double invHeight = 1.0f / mode->height;

		while (!glfwWindowShouldClose(window)) {
			glfwPollEvents();

			Time::time = glfwGetTime();
			Time::deltaTime = Time::time - previousTime;
			Time::fps = 1.0f / Time::deltaTime;
			previousTime = Time::time;

			static Vector2f previousMouse;

			glfwGetCursorPos(window, &Input::mouseX, &Input::mouseY);
			Input::mousePosition = Vector2f((Input::mouseX * invWidth) * 2.0f - 1.0f, 1.0f - (Input::mouseY * invHeight) * 2.0f);
			Input::mouseDelta = Input::mousePosition - previousMouse;
			previousMouse = Input::mousePosition;

			update();
		}

		end();
	}

	virtual ~AppBase() {}

	virtual void start() {}
	virtual void update() {}
	virtual void end() {}

	ScriptHandler scriptHandler;
	ShaderProgram shader;

private:
	double previousTime;

	friend class BackendInit;
};

}

}

#endif