#ifndef CT_SHADER_HPP
#define CT_SHADER_HPP

#include <mstd/misc>
#include <glad/glad.h>
#include <vector>
#include <string>

namespace ct {

class Shader {
public:
	Shader() {}

	struct CreateInfo {
		const mstd::C8* vertexPath;
		const mstd::C8* tesselationControlPath;
		const mstd::C8* tesselationEvaluationPath;
		const mstd::C8* geometryPath;
		const mstd::C8* fragmentPath;
	};

	Shader(const Shader& shader) = delete;
	Shader& operator=(const Shader& shader) = delete;

	Shader(Shader&& shader) = default;

	Shader& operator=(Shader&& shader) {
		this->~Shader();

		this->shaderProgram = shader.shaderProgram;
		shader.shaderProgram = 0;

		return *this;
	}

	Shader(CreateInfo createInfo);

	~Shader() {
		if (shaderProgram) {
			glDeleteProgram(shaderProgram);
			shaderProgram = 0;
		}
	}

	operator GLuint() const {
		return shaderProgram;
	}

	operator mstd::Bool() const {
		return shaderProgram;
	}

private:
	GLuint shaderProgram = 0;
};

}

#endif
