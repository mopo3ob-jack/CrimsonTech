#include "crimson.hpp"
#include "scripts/scripts.hpp"

#include <GLFW/glfw3.h>
#include <stdio.h>
#include <fcntl.h>
#include <unistd.h>

using namespace hdb;
using namespace hdb::ct;

class CrimsonTest : public AppBase {
public:
	void start() override {
		using namespace Context;

		shader = ShaderProgram("./shaders/shader.vert", "./shaders/shader.frag");

		glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);

		glViewport(0, 0, mode->width, mode->height);
		glEnable(GL_DEPTH_TEST);
		glDepthFunc(GL_LESS);
		glEnable(GL_CULL_FACE);
		glCullFace(GL_CCW);

		camera = Camera(shader.getShaderProgram());
		camera.fov = M_PI * 0.75f;
		camera.position = Vector3(0.0f);
		camera.rotation = Vector3(0.0f);
		aspectRatio = (float)mode->width / (float)mode->height;
		camera.position.z = -2.0f;

		cameraController.camera = &camera;

		//Starting threads
		scriptHandler.resizeThreads(2);
		scriptHandler.addScript(&cameraController);

		Vertex cubeVertices[] = {
			{{-1.0f, -1.0f, -1.0f}, {}, {255, 0, 0}, {}, {}, {}},
			{{1.0f, -1.0f, -1.0f}, {}, {0, 255, 0}, {}, {}, {}},
			{{1.0f, 1.0f, -1.0f}, {}, {0, 0, 255}, {}, {}, {}},
			{{-1.0f, 1.0f, -1.0f}, {}, {255, 255, 0}, {}, {}, {}},
			{{1.0f, -1.0f, 1.0f}, {}, {0, 255, 255}, {}, {}, {}},
			{{-1.0f, -1.0f, 1.0f}, {}, {255, 255, 255}, {}, {}, {}},
			{{-1.0f, 1.0f, 1.0f}, {}, {255, 255, 0}, {}, {}, {}},
			{{1.0f, 1.0f, 1.0f}, {}, {0, 0, 0}, {}, {}, {}}
		};

		unsigned int cubeIndices[] = {
			0, 1, 2,	0, 2, 3,	//Front
			4, 5, 6,	4, 6, 7,	//Back
			5, 0, 3,	5, 3, 6,	//Left
			1, 4, 7,	1, 7, 2,	//Right
			5, 4, 1,	5, 1, 0,	//Bottom
			3, 2, 7,	3, 7, 6		//Top
		};

		cube.allocateBuffers(1);
		cube.vertexData(cubeVertices, 8, GL_STATIC_DRAW);
		cube.indexData(0, cubeIndices, 36, GL_STATIC_DRAW);
	}

	void update() override {
		using namespace Context;

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, true);
		}

		scriptHandler.execute();

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		shader.useProgram();
		camera.updateUniform(aspectRatio, 0.1f, 914.4f);

		cube.render(0);

		glfwSwapBuffers(window);
	}

	void end() override {
	}

	Camera camera;
	float aspectRatio;
	
	//Renderables
	RenderBatch cube;

	//Scripts
	CameraController cameraController;
};

int main(int argc, char** argv) {
	glfwInit();

	Context::monitor = glfwGetPrimaryMonitor();
	Context::mode = glfwGetVideoMode(Context::monitor);
	initCrimson(Context::mode->width, Context::mode->height, "Crimson Test", Context::monitor);

	runCrimsonApp<CrimsonTest>();

	terminateCrimson();

	return 0;
}

// #include "crimson.hpp"

// using namespace hdb::ct;

// class A : public ScriptBase {
// public:
// 	unsigned int i = 0;
// 	void start() {
// 		printf("A Start\n");
// 	}

// 	void update() {
// 		++i;
// 	}

// 	void end() {
// 		printf("A End %d\n", i);
// 	}
// };

// class B : public ScriptBase {
// public:
// 	unsigned int i = 0;
// 	void start() {
// 		printf("B Start\n");
// 	}

// 	void update() {
// 		++i;
// 	}

// 	void end() {
// 		printf("B End %d\n", i);
// 	}
// };

// int main() {
// 	A a;
// 	B b;

// 	ScriptHandler handler(2);
// 	handler.addScript(&a);
// 	handler.addScript(&b);
	
// 	for (unsigned int i = 0; i < 100000; ++i) {
// 		handler.execute();
// 	}

// 	return 0;
// }