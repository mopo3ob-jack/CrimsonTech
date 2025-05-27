#include <mstd/geometry>
#include <mstd/misc>

#include <crimson/renderer>
#include <crimson/types.d/Plane.hpp>

#include <GLFW/glfw3.h>

#include <iostream>
#include <GL/glu.h>

using namespace mstd;
using namespace ct;

static void resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
}

int main() {
	glfwInit();
	
	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);
	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "Cube", monitor, nullptr);

	glfwMakeContextCurrent(window);
	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << "Failed to initialize GLAD\n" << std::endl;
		return 1;
	}

	glfwSetFramebufferSizeCallback(window, resize);

	Shader::CreateInfo createInfo = {
		.vertexPath = "resources/shaders/shader.vert",
		.fragmentPath = "resources/shaders/shader.frag",
	};
	Shader shader(createInfo);

	shader.use();
	GLuint objectUniform = glGetUniformLocation(shader, "object");
	GLuint cameraUniform = glGetUniformLocation(shader, "camera");

	std::vector<VertexArray::Attribute> vertexAttributes = {
		{
			.stride = sizeof(Vector3f),
			.size = 3,
			.type = GL_FLOAT,
			.normalized = GL_FALSE
		},
		{
			.stride = sizeof(Vector3f),
			.size = 3,
			.type = GL_FLOAT,
			.normalized = GL_FALSE
		}
	};
	VertexArray vertexArray(vertexAttributes);

	Vector3f vertices[] = {
		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f}
	};
	Vector3f colors[] = {
		{1.0, 0.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 0.0, 1.0}
	};
	mstd::U16 indices[] = {
		0, 1, 2
	};
	vertexArray.allocateAttributes(0, vertices, 3, GL_STATIC_DRAW);
	vertexArray.allocateAttributes(1, colors, 3, GL_STATIC_DRAW);
	vertexArray.allocateElements(indices, 3, GL_STATIC_DRAW);

	bool isFullscreen = true;
	bool f11Down = false;

	glViewport(0, 0, mode->width, mode->height);
	glClearColor(0.0, 0.0, 0.0, 1.0);

	F32 previousTime = glfwGetTime();
	F32 deltaTime;
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		deltaTime = glfwGetTime() - previousTime;
		previousTime = glfwGetTime();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			break;
		}

		if (glfwGetKey(window, GLFW_KEY_F11) == GLFW_PRESS) {
			if (f11Down == false) {
				if (isFullscreen) {
					glfwSetWindowMonitor(window, monitor, 0, 0, mode->width, mode->height, 0);
				}
				else {
					glfwSetWindowMonitor(window, nullptr, mode->width / 2 - 400, mode->height / 2 - 300, 800, 600, 0);
				}

				isFullscreen = !isFullscreen;
			}
			
			f11Down = true;
		} else {
			f11Down = false;
		}

		Rotorf rotor({1.0, 0.0, 0.0}, {std::cos(previousTime * 0.1f), std::sin(previousTime * 0.1f), 0.0});
		Matrix3f mat = rotor;
		Vector3f transformed[sizeof(vertices) / sizeof(Vector3f)];
		for (U32 i = 0; i < 3; ++i) {
			transformed[i] = mat * vertices[i];

			colors[i].r = std::cos(i * 2.0 + previousTime);
			colors[i].g = std::sin(i * 2.0 + previousTime);
			colors[i].b = -std::cos(i * 2.0 + previousTime);
		}

		//vertexArray.writeAttributes(0, transformed, 3);
		vertexArray.writeAttributes(1, colors, 3);

		Matrix4f objectMatrix = translate<F32, 4>(Vector3f(0.0, 0.0, 3.0));
		Matrix4f cameraMatrix = 
			translate<F32, 4>(Vector3f(std::cos(previousTime), 0.0, 0.0))
			*perspective<F32>(F32(mode->width) / F32(mode->height), M_PI_2, 0.01, 100.0)
		;

		glClear(GL_COLOR_BUFFER_BIT);
		shader.use();
		glUniformMatrix4fv(objectUniform, 1, GL_FALSE, (GLfloat*)&objectMatrix);
		glUniformMatrix4fv(cameraUniform, 1, GL_TRUE, (GLfloat*)&cameraMatrix);
		vertexArray.draw<mstd::U16>(3L, 0L);
		glfwSwapBuffers(window);
	}

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
