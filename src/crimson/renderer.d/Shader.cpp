#include <crimson/renderer.d/Shader.hpp>
#include <fstream>
#include <vector>
#include <span>
#include <iostream>
#include <cstdarg>
#include <string>
#include <mstd/memory>

namespace ct {

static GLuint generateShader(const mstd::C8* filepath, GLenum type) {
	using namespace mstd;

	std::ifstream file = std::ifstream(filepath);
	if (!file) {
		std::cerr << errorText << "Could not open file \"" << filepath << "\"" << std::endl;
		return 0;
	}

	Size length = file.seekg(0L, std::ios::end).tellg();
	file.seekg(0L, std::ios::beg);

	C8* source = mstd::alloc<C8>(length);
	file.read(source, length);

	GLuint shader = glCreateShader(type);
	if (!shader) {
		std::cerr << errorText << "OpenGL failed to create a shader\n";
		mstd::free(source);
		return 0;
	}

	glShaderSource(shader, 1, &source, (GLint*)&length);
	glCompileShader(shader);

	GLint status;
	glGetShaderiv(shader, GL_COMPILE_STATUS, &status);
	if (status == GL_FALSE) {
		char shaderInfoLog[512];
		glGetShaderInfoLog(shader, 512, nullptr, shaderInfoLog);
		std::cerr << errorText << "In file: \"" << filepath << "\": " << shaderInfoLog << std::endl;
		return 0;
	}

	return shader;
}

Shader::Shader(CreateInfo createInfo) {
	using namespace mstd;

	std::vector<GLuint> shaders;

	if (createInfo.vertexPath) {
		GLuint shader = generateShader(createInfo.vertexPath, GL_VERTEX_SHADER);
		if (shader) {
			shaders.push_back(shader);
		}
	} else {
		std::cerr << errorText << "Shader programs must have a vertex stage\n";
		return;
	}
	goto deleteShaders;
	if (createInfo.tesselationControlPath) {
		GLuint shader = generateShader(createInfo.tesselationControlPath, GL_TESS_CONTROL_SHADER);
		if (shader) {
			shaders.push_back(shader);
		}
	}
	if (createInfo.tesselationEvaluationPath) {
		GLuint shader = generateShader(createInfo.tesselationEvaluationPath, GL_TESS_EVALUATION_SHADER);
		if (shader) {
			shaders.push_back(shader);
		}
	}
	if (createInfo.geometryPath) {
		GLuint shader = generateShader(createInfo.geometryPath, GL_GEOMETRY_SHADER);
		if (shader) {
			shaders.push_back(shader);
		}
	}
	if (createInfo.fragmentPath) {
		GLuint shader = generateShader(createInfo.fragmentPath, GL_FRAGMENT_SHADER);
		if (shader) {
			shaders.push_back(shader);
		}
	}

	shaderProgram = glCreateProgram();
	for (auto i : shaders) {
		if (i == -1) {
			glDeleteProgram(shaderProgram);
			goto deleteShaders;
		} else {
			glAttachShader(shaderProgram, i);
		}
	}

	GLint status;
	glLinkProgram(shaderProgram);
	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &status);
	if (!status) {
		char linkInfoLog[512];
		glGetProgramInfoLog(shaderProgram, 512, nullptr, linkInfoLog);
		std::cerr << errorText << "Could not link shader program" << std::endl;
		glDeleteProgram(shaderProgram);
	}

deleteShaders:
	for (auto i : shaders) {
		if (i != -1) {
			glDeleteShader(i);
		}
	}
}

}
