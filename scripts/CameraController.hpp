#ifndef HDB_CT_SCRIPTS_CAMERACONTROLLER_HPP
#define HDB_CT_SCRIPTS_CAMERACONTROLLER_HPP

#include "../crimson.hpp"

namespace hdb {

namespace ct {

class CameraController : public ScriptBase {
public:
	void start() {
		camera->fov = M_PI * 0.75f;
		camera->position = Vector3(0.0f, 0.0f, -1.0f);
		camera->rotation = Vector3(0.0f);

		speed = 1.0f;
		sensitivity = 1.8f;
	}

	void update() {
		using namespace Context;

		if (glfwGetKey(window, GLFW_KEY_S)) {
			camera->position.z -= cos(camera->rotation.y) * Time::deltaTime * speed;
			camera->position.x += sin(camera->rotation.y) * Time::deltaTime * speed;
		}

		if (glfwGetKey(window, GLFW_KEY_W)) {
			camera->position.z += cos(camera->rotation.y) * Time::deltaTime * speed;
			camera->position.x -= sin(camera->rotation.y) * Time::deltaTime * speed;
		}

		if (glfwGetKey(window, GLFW_KEY_A)) {
			camera->position.z -= sin(camera->rotation.y) * Time::deltaTime * speed;
			camera->position.x -= cos(camera->rotation.y) * Time::deltaTime * speed;
		}

		if (glfwGetKey(window, GLFW_KEY_D)) {
			camera->position.z += sin(camera->rotation.y) * Time::deltaTime * speed;
			camera->position.x += cos(camera->rotation.y) * Time::deltaTime * speed;
		}

		if (glfwGetKey(window, GLFW_KEY_Q)) {
			camera->position.y -= Time::deltaTime * speed;
		}

		if (glfwGetKey(window, GLFW_KEY_E)) {
			camera->position.y += Time::deltaTime * speed;
		}

		camera->rotation.x += Input::mouseDelta.y * sensitivity;
		camera->rotation.y -= Input::mouseDelta.x * sensitivity;

		if (camera->rotation.x > M_PI_2)
			camera->rotation.x = M_PI_2;
		if (camera->rotation.x < -M_PI_2)
			camera->rotation.x = -M_PI_2;
	}

	void end() {

	}

	Camera* camera;
	float speed;
	float sensitivity;
};

}

}

#endif