#include <glad/glad.h>
#include <GLFW/glfw3.h>
#include <crimson/renderer>
#include <mstd/geometry>

void resize(GLFWwindow* window, mstd::I32 width, mstd::I32 height);

int main() {
	using namespace mstd;
	using namespace ct;

	glfwInit();

	glfwWindowHint(GLFW_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_VERSION_MINOR, 6);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "texture", monitor, nullptr);

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << errorText << "Could not initialize GLAD" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwSetWindowSizeCallback(window, resize);
	resize(window, mode->width, mode->height);

	Shader::CreateInfo shaderCreateInfo = {
		.vertexPath = "resources/shaders/shader.vert",
		.fragmentPath = "resources/shaders/shader.frag",
	};
	Shader shader(shaderCreateInfo);
	shader.use();

	std::vector<VertexArray::Attribute> attributes = {
		{
			.stride = sizeof(Vector2f),
			.size = 2,
			.type = GL_FLOAT,
			.normalized = false
		},
		{
			.stride = sizeof(Vector2f),
			.size = 2,
			.type = GL_FLOAT,
			.normalized = false
		}
	};

	std::vector<Vector2f> vertices = {
		{-1.0f, -1.0f},
		{1.0f, -1.0f},
		{1.0f, 1.0f},
		{-1.0f, 1.0f},
	};

	std::vector<Vector2f> uvs = {
		{0.0f, 0.0f},
		{1.0f, 0.0f},
		{1.0f, 1.0f},
		{0.0f, 1.0f},
	};

	std::vector<U16> indices = {
		0, 1, 2,
		0, 2, 3
	};

	VertexArray tile(attributes, vertices.size(), indices.size(), sizeof(U16));
	tile.writeAttributes(0, vertices.data(), vertices.size());
	tile.writeAttributes(1, uvs.data(), uvs.size());
	tile.writeElements(indices.data(), indices.size());
	
	while (!glfwWindowShouldClose(window)) {
		glfwPollEvents();

		glClear(GL_COLOR_BUFFER_BIT);

		tile.draw<U16>(indices.size(), 0);
		
		glfwSwapBuffers(window);
	}

	glfwTerminate();

	return 0;
}

void resize(GLFWwindow* window, mstd::I32 width, mstd::I32 height) {
	glViewport(0, 0, width, height);
}
