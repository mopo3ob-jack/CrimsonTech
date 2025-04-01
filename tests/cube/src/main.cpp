#include <mstd/geometry>
#include <mstd/misc>

#include <crimson/renderer>

#include <GLFW/glfw3.h>

#include <random>
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
	gladLoadGLLoader((GLADloadproc)glfwGetProcAddress);

	Shader::CreateInfo createInfo = {
		.vertexPath = "resources/shaders/shader.vert",
		.fragmentPath = "resources/shaders/shader.frag",
	};
	Shader shader(createInfo);

	std::vector<VertexArray::Attribute> vertexAttributes = {
		{
			.size = 3,
			.type = GL_FLOAT,
			.normalized = GL_FALSE,
			.stride = sizeof(Vector3f)
		}
	};
	VertexArray vertexArray(vertexAttributes);

	bool isFullscreen = true;
	bool f11Down = false;


	glfwSetFramebufferSizeCallback(window, resize);

	glViewport(0, 0, mode->width, mode->height);
	glClearColor(0.674509804f, 0.988235294f, 0.95686274509f, 1.0f);

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

		glClear(GL_COLOR_BUFFER_BIT);
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}
