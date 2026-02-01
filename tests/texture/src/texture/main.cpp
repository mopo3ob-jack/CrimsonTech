#include <crimson/renderer.d/Context.hpp>
#include <crimson/renderer>
#include <crimson/game>
#include <mstd/geometry>
#include <stb/stb_image.h>
#include <algorithm>

using namespace mstd;
using namespace ct;

void resize(GLFWwindow* window, mstd::I32 width, mstd::I32 height);

static constexpr Vector2f sensitivity(0.5f, 0.5f);

static constexpr F32 maxVelocity = 6.0f;
static constexpr F32 maxAcceleration = 40.0;
static constexpr F32 frictionCoefficient = 1.7f;

class Game : public Context {
public:
	Game(const C8* title) : Context(title) {}

	~Game() {}

	void init() override {
		Shader::CreateInfo shaderCreateInfo = {
			.vertexPath = "resources/shaders/shader.vert",
			.fragmentPath = "resources/shaders/shader.frag",
		};
		shader = Shader(shaderCreateInfo);
		shader.use();

		std::vector<VertexArray::Attribute> attributes = {
			{
				.stride = sizeof(Vector3f),
				.size = 3,
				.type = GL_FLOAT,
				.normalized = false
			},
			{
				.stride = sizeof(Vector3f),
				.size = 3,
				.type = GL_FLOAT,
				.normalized = false
			},
			{
				.stride = sizeof(Vector3f),
				.size = 3,
				.type = GL_FLOAT,
				.normalized = false
			},
			{
				.stride = sizeof(Vector3f),
				.size = 3,
				.type = GL_FLOAT,
				.normalized = false
			},
			{
				.stride = sizeof(Vector2f),
				.size = 2,
				.type = GL_FLOAT,
				.normalized = false
			},
			{
				.stride = sizeof(U32),
				.size = 1,
				.type = GL_UNSIGNED_INT,
				.normalized = false,
				.integral = true
			}
		};

		std::vector<Vector3f> vertices = {
			{-1.0f, -1.0f, 0.0f},
			{1.0f, -1.0f, 0.0f},
			{1.0f, 1.0f, 0.0f},
			{-1.0f, 1.0f, 0.0f},

			{-1.0f, -1.0f, 0.0f},
			{1.0f, -1.0f, 0.0f},
			{1.0f, -1.0f, 2.0f},
			{-1.0f, -1.0f, 2.0f},
		};

		std::vector<Vector3f> normals = {
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f},

			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
		};

		std::vector<Vector3f> tangents = {
			{1.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},
			{1.0f, 0.0f, 0.0f},

			{-1.0f, 0.0f, 0.0f},
			{-1.0f, 0.0f, 0.0f},
			{-1.0f, 0.0f, 0.0f},
			{-1.0f, 0.0f, 0.0f},
		};

		std::vector<Vector3f> bitangents = {
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},
			{0.0f, 1.0f, 0.0f},

			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f},
			{0.0f, 0.0f, 1.0f},
		};

		std::vector<Vector2f> uvs = {
			{0.0f, 0.0f},
			{1.0f, 0.0f},
			{1.0f, 1.0f},
			{0.0f, 1.0f},

			{0.0f, 1.0f},
			{1.0f, 1.0f},
			{1.0f, 0.0f},
			{0.0f, 0.0f},
		};

		std::vector<GLuint> materialIndices = {
			0,
			0, 
			0, 
			0,

			0,
			0, 
			0, 
			0
		};

		VertexArray::ConvertOptions convertOptions = {
			.positions = 0,
			.materials = 5,
			.uvs = 4,
			.normals = 1,
			.tangents = 2,
			.bitangents = 3
		};
		//VertexArray::convert("resources/models/room.fbx", "resources/models/room.vac", convertOptions, arena);
		//stage.load("resources/models/room.vac", vertexCount, elementCount, arena);

		tile = VertexArray(attributes, vertices.size(), indices.size(), sizeof(U16));
		tile.writeAttributes(0, vertices.data(), vertices.size());
		tile.writeAttributes(1, normals.data(), normals.size());
		tile.writeAttributes(2, tangents.data(), tangents.size());
		tile.writeAttributes(3, bitangents.data(), bitangents.size());
		tile.writeAttributes(4, uvs.data(), uvs.size());
		tile.writeAttributes(5, materialIndices.data(), materialIndices.size());
		tile.writeElements(indices.data(), indices.size());

		struct Material {
			U32 albedoIndex, albedoUnit;
			U32 normalIndex, normalUnit;
		};

		objectUniform = glGetUniformLocation(shader, "object");
		rotationUniform = glGetUniformLocation(shader, "rotate");
		cameraUniform = glGetUniformLocation(shader, "camera");
		cameraPositionUniform = glGetUniformLocation(shader, "cameraPosition");

		I32 texWidth, texHeight, texComp;
		auto albedo = stbi_load("resources/textures/albedo.png", &texWidth, &texHeight, &texComp, 4);
		auto normal = stbi_load("resources/textures/normal.png", &texWidth, &texHeight, &texComp, 4);

		U32 widthLog = std::log2(texWidth) - 3;

		U32 textureCount[TextureManager::TEXTURE_UNITS] = {
			0, 0, 0, 0, 0, 0, 2, 2, 2, 2
		};

		std::cout << widthLog << std::endl;
		tm = TextureManager(textureCount, shader);
		tm.write(widthLog, 0, albedo, texWidth);
		tm.write(widthLog, 1, normal, texWidth);

		stbi_image_free(albedo);
		stbi_image_free(normal);

		std::vector<Material> materials = {
			{0, widthLog, 1, widthLog},
		};

		materialBuffer = StorageBuffer(materials.data(), materials.size());
		materialBuffer.bind(0);

		player.position = Vector3f(0.0f, 1.0f, 1.5f);
		player.angle = Vector3f(0.0f, 0.0f, M_PI);

		glfwGetCursorPos(window, &prevMousePosition.x, &prevMousePosition.y);
	}

	void update() override {
		Vector2d mouseDelta;

		Time::update();

		if (glfwGetKey(window, GLFW_KEY_ESCAPE)) {
			glfwSetWindowShouldClose(window, GLFW_TRUE);
			return;
		}

		Vector2d mousePosition;
		glfwGetCursorPos(window, &mousePosition.x, &mousePosition.y);
		mouseDelta = mousePosition - prevMousePosition;
		player.angle.x += mouseDelta.y * sensitivity.y / F32(mode->height);
		player.angle.z += mouseDelta.x * sensitivity.x / F32(mode->height);

		player.angle.x = std::clamp(player.angle.x, F32(-M_PI_2), F32(M_PI_2));
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
		
		player.update();

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

		tm.bind();
		materialBuffer.bind(0);

		tile.draw<U16>(indices.size(), 0);
		//stage.draw<U32>(elementCount, 0);
	}

	void end() override {
	}

	void resize(I32 width, I32 height) override {
		glViewport(0, 0, width, height);
		projectionMatrix = perspective(F32(width) / F32(height), F32(M_PI_2), 0.015625f, 128.0f);
	}

	std::vector<U16> indices = {
		0, 1, 2,
		0, 2, 3,

		4, 5, 6,
		4, 6, 7,
	};

	U32 vertexCount, elementCount;

	Arena arena = Arena(1ull << 24);
	Shader shader;
	VertexArray stage;
	VertexArray tile;
	StorageBuffer materialBuffer;
	TextureManager tm;

	GLuint objectUniform, rotationUniform, cameraUniform, cameraPositionUniform;
	
	FreeCamera player;
	Vector2d prevMousePosition;

	Matrix4f projectionMatrix;
};

int main() {
	Game game("texture");
	game.run();

	return 0;
}
