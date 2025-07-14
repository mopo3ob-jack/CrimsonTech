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

static Matrix4f rotateX(F32 angle) {
	F32 cosA = std::cos(angle);
	F32 sinA = std::sin(angle);

	return Matrix4f(
		{1,  0,    0,     0},
		{0,  cosA, -sinA, 0},
		{0,  sinA, cosA,  0},
		{0,  0,    0,     1}
	);
}

static Matrix4f rotateY(F32 angle) {
	F32 cosA = std::cos(angle);
	F32 sinA = std::sin(angle);

	return Matrix4f(
		{cosA,  0, sinA,  0},
		{0,     1, 0,     0},
		{-sinA, 0, cosA,  0},
		{0,     0, 0,     1}
	);
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

	BSP bsp;
	bsp.build("resources/models/room.fbx");

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
		},
	};

	Vector3f vertices[] = {
		{1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},

		{-1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},

		{1.0f, -1.0f, -1.0f},
		{1.0f, -1.0f, 1.0f},
		{1.0f, 1.0f, 1.0f},
		{1.0f, 1.0f, -1.0f},

		{-1.0f, -1.0f, 1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, 1.0f, -1.0f},
		{-1.0f, 1.0f, 1.0f},

		{-1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, -1.0f},
		{1.0f, 1.0f, 1.0f},
		{-1.0f, 1.0f, 1.0f},

		{1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, -1.0f},
		{-1.0f, -1.0f, 1.0f},
		{1.0f, -1.0f, 1.0f},
	};

	Vector3f normals[] = {
		{0.0, 0.0, 1.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, 1.0},
		{0.0, 0.0, 1.0},

		{0.0, 0.0, -1.0},
		{0.0, 0.0, -1.0},
		{0.0, 0.0, -1.0},
		{0.0, 0.0, -1.0},

		{1.0, 0.0, 0.0},
		{1.0, 0.0, 0.0},
		{1.0, 0.0, 0.0},
		{1.0, 0.0, 0.0},

		{-1.0, 0.0, 0.0},
		{-1.0, 0.0, 0.0},
		{-1.0, 0.0, 0.0},
		{-1.0, 0.0, 0.0},

		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},
		{0.0, 1.0, 0.0},

		{0.0, -1.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, -1.0, 0.0},
		{0.0, -1.0, 0.0},
	};

	U16 indices[] = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,

		8, 9, 10,
		8, 10, 11,

		12, 13, 14,
		12, 14, 15,

		16, 17, 18,
		16, 18, 19,

		20, 21, 22,
		20, 22, 23,
	};
	
	VertexArray vertexArray;

	vertexArray.allocateAttributes(vertexAttributes, sizeof(vertices) / sizeof(Vector3f));
	vertexArray.writeAttributes(0, vertices, sizeof(vertices) / sizeof(Vector3f));
	vertexArray.writeAttributes(1, normals, sizeof(normals) / sizeof(Vector3f));

	vertexArray.allocateElements(sizeof(indices));
	vertexArray.writeElements(indices, sizeof(indices) / sizeof(U16));

	Player player;
	player.position = Vector3f(0.0f, 1.0f, 0.0f);
	player.velocity = Vector3f(0.0f, 0.0f, 0.0f);
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
			player.angle.x += mouseDelta.y * sensitivity.y / F32(mode->height);
			player.angle.y += mouseDelta.x * sensitivity.x / F32(mode->height);

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
		
		if (glfwGetKey(window, GLFW_KEY_SPACE)) {
			player.keyMask |= player.JUMP;
		}
		
		player.update(bsp);

		glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

		Matrix4f rotationMatrix = 
			rotateX(Time::time)
			* rotateY(Time::time);

		Matrix4f objectMatrix = translate(Vector3f(0.0, 1.0, 0.0)) * rotationMatrix;

		Matrix4f cameraMatrix = 
			projectionMatrix
			* rotateX(player.angle.x)
			* rotateY(player.angle.y)
			* translate(-player.position);

		shader.use();

		glUniformMatrix4fv(objectUniform, 1, GL_FALSE, (GLfloat*)&objectMatrix);
		glUniformMatrix4fv(rotationUniform, 1, GL_FALSE, (GLfloat*)&rotationMatrix);
		glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, (GLfloat*)&cameraMatrix);
		glUniform3fv(cameraPositionUniform, 1, (GLfloat*)&player.position);

		rotationMatrix = rotateX(0.0f);

		objectMatrix = translate(Vector3f(0.0, 0.0, 0.0)) * rotationMatrix;

		Matrix4f identityMatrix = translate(Vector3f(0.0f));
		glUniformMatrix4fv(objectUniform, 1, GL_FALSE, (GLfloat*)&objectMatrix);
		glUniformMatrix4fv(rotationUniform, 1, GL_FALSE, (GLfloat*)&rotationMatrix);
		glUniformMatrix4fv(cameraUniform, 1, GL_FALSE, (GLfloat*)&cameraMatrix);
		glUniform3fv(cameraPositionUniform, 1, (GLfloat*)&player.position);

		bsp.vertexArray.draw<U32>(bsp.indexCount, 0);

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
