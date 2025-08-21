#include <mstd/geometry>
#include <mstd/misc>

#include <crimson/renderer>
#include <crimson/file>
#include <crimson/game>
#include <crimson/types.d/Plane.hpp>

#include <GLFW/glfw3.h>
#include <imgui/imgui.h>
#include <imgui/imgui_impl_glfw.h>
#include <imgui/imgui_impl_opengl3.h>

#include <iostream>
#include <algorithm>
#include <GL/glu.h>

using namespace mstd;
using namespace ct;

static constexpr Vector2f sensitivity(0.5f, 0.5f);

static constexpr F32 maxVelocity = 6.0f;
static constexpr F32 maxAcceleration = 40.0;
static constexpr F32 frictionCoefficient = 1.7f;

static Matrix4f projectionMatrix;

static void resize(GLFWwindow* window, int width, int height) {
	glViewport(0, 0, width, height);
	projectionMatrix = perspective(F32(width) / F32(height), F32(M_PI_2), 0.01f, 100.0f);
}

int main() {
	glfwInit();

	glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 4);
	glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 6);
	glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);

	GLFWmonitor* monitor = glfwGetPrimaryMonitor();
	const GLFWvidmode* mode = glfwGetVideoMode(monitor);
	GLFWwindow* window = glfwCreateWindow(mode->width, mode->height, "BSP", monitor, nullptr);
	if (!window) {
		std::cerr << errorText << "Could not open a GLFW window" << std::endl;
		glfwTerminate();
		return 1;
	}

	glfwMakeContextCurrent(window);

	if (!gladLoadGLLoader((GLADloadproc)glfwGetProcAddress)) {
		std::cerr << errorText << "Could not initialize GLAD" << std::endl;
		glfwTerminate();
		return 1;
	}

	projectionMatrix = perspective(F32(mode->width) / F32(mode->height), F32(M_PI_2), 0.01f, 100.0f);

	glfwSetFramebufferSizeCallback(window, resize);
	Bool cursorEnabled = false;
	glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
	if (glfwRawMouseMotionSupported()) {
		glfwSetInputMode(window, GLFW_RAW_MOUSE_MOTION, GLFW_TRUE);
	}

	IMGUI_CHECKVERSION();
	ImGui::CreateContext();
	ImGuiIO& io = ImGui::GetIO();

	ImGui_ImplGlfw_InitForOpenGL(window, true);
	ImGui_ImplOpenGL3_Init();

	glfwSwapInterval(1);

	glViewport(0, 0, mode->width, mode->height);
	glClearColor(0.0, 0.0, 0.0, 1.0);
	glCullFace(GL_BACK);
	glFrontFace(GL_CCW);
	glEnable(GL_CULL_FACE);
	glEnable(GL_DEPTH_TEST);
	//glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);  

	Shader::CreateInfo createInfo = {
		.vertexPath = "resources/shaders/shader.vert",
		.fragmentPath = "resources/shaders/shader.frag",
	};

	Shader shader(createInfo);
	shader.use();
	GLuint objectUniform = glGetUniformLocation(shader, "object");
	GLuint rotationUniform = glGetUniformLocation(shader, "rotate");
	GLuint cameraUniform = glGetUniformLocation(shader, "camera");
	GLuint cameraPositionUniform = glGetUniformLocation(shader, "cameraPosition");
	GLuint wireframeUniform = glGetUniformLocation(shader, "wireframe");

	Arena arena((Size)1 << 32);
	BSP bsp;
	bsp.build("resources/models/room.fbx", arena);

	FlyCamera player;
	player.position = Vector3f(0.0f, 2.0f, 1.5f);
	//player.velocity = Vector3f(0.0f, 0.0f, 0.0f);
	player.angle = Vector3f(0.0f);

	Vector2d prevMousePosition;
	glfwGetCursorPos(window, &prevMousePosition.x, &prevMousePosition.y);
	Vector2d mouseDelta;

	Size imguiFrames = 0;
	do {
		glfwPollEvents();
		Time::update();

		ImGui_ImplOpenGL3_NewFrame();
		ImGui_ImplGlfw_NewFrame();
		ImGui::NewFrame();

		if (glfwGetKey(window, GLFW_KEY_TAB)) {
			++imguiFrames;
		} else {
			imguiFrames = 0;
		}

		if (imguiFrames == 1) {
			if (cursorEnabled) {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_DISABLED);
				io.ConfigFlags &= ~ImGuiConfigFlags_NavEnableKeyboard;
				io.ConfigFlags |= ImGuiConfigFlags_NoMouse;
			} else {
				glfwSetInputMode(window, GLFW_CURSOR, GLFW_CURSOR_NORMAL);
				io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
				io.ConfigFlags &= ~ImGuiConfigFlags_NoMouse;
			}

			cursorEnabled ^= 1;
		}

		Vector2d mousePosition;
		glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
		if (!cursorEnabled) {
			mouseDelta = mousePosition - prevMousePosition;
			player.angle.x -= mouseDelta.y * sensitivity.y / F32(mode->height);
			player.angle.y -= mouseDelta.x * sensitivity.x / F32(mode->height);

			player.angle.x = std::clamp(player.angle.x, F32(-M_PI_2), F32(M_PI_2));
		}
		prevMousePosition = mousePosition;

		player.keyMask = 0;
		if (glfwGetKey(window, GLFW_KEY_W)) {
			player.keyMask |= player.FORWARD;
		}

		if (glfwGetKey(window, GLFW_KEY_S)) {
			player.keyMask |= player.BACKWARD;
		}
		
		if (glfwGetKey(window, GLFW_KEY_D)) {
			player.keyMask |= player.RIGHT;
		}
		
		if (glfwGetKey(window, GLFW_KEY_A)) {
			player.keyMask |= player.LEFT;
		}
		
		if (glfwGetKey(window, GLFW_KEY_E)) {
			player.keyMask |= player.UP;
		}

		if (glfwGetKey(window, GLFW_KEY_Q)) {
			player.keyMask |= player.DOWN;
		}
		
		player.update(bsp);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Matrix4f rotationMatrix = Matrix4f(1.0f);
		Matrix4f objectMatrix = translate(Vector3f(0.0, 1.0, 0.0)) * rotationMatrix;

		Matrix4f cameraMatrix = projectionMatrix * player.viewMatrix;

		shader.use();

		rotationMatrix = rotateX(0.0f);

		objectMatrix = translate(Vector3f(0.0, 0.0, 0.0)) * rotationMatrix;

		Matrix4f identityMatrix = translate(Vector3f(0.0f));
		glUniformMatrix4fv(objectUniform, 1, GL_FALSE, (GLfloat*)&objectMatrix);
		glUniformMatrix4fv(rotationUniform, 1, GL_FALSE, (GLfloat*)&rotationMatrix);
		glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, (GLfloat*)&cameraMatrix);
		glUniform3fv(cameraPositionUniform, 1, (GLfloat*)&player.position);

		glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		glUniform1i(wireframeUniform, 0);
		bsp.vertexArray.draw<U32>(bsp.elementCount, 0);

		glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		glClear(GL_DEPTH_BUFFER_BIT);
		glUniform1i(wireframeUniform, 1);
		bsp.vertexArray.draw<U32>(bsp.elementCount, 0);

		ImGui::Render();
		ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

		glfwSwapBuffers(window);
	} while (!glfwWindowShouldClose(window));

	ImGui_ImplOpenGL3_Shutdown();
	ImGui_ImplGlfw_Shutdown();
	ImGui::DestroyContext();

	glfwDestroyWindow(window);
	glfwTerminate();

	return 0;
}
