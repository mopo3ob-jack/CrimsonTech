#include "../../include/render/ShaderProgram.hpp"

#include "../../include/file/file.hpp"

#include <fcntl.h>
#include <unistd.h>

#include <stdio.h>

namespace hdb {

namespace ct {

ShaderProgram::ShaderProgram() {}

ShaderProgram::ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath) {
	this->~ShaderProgram();

	MappedFile vertexFile(vertexShaderPath, O_RDONLY, PROT_READ);
	MappedFile fragmentFile(fragmentShaderPath, O_RDONLY, PROT_READ);

	int success;
	char log[1024];
	GLsizei logLength = 0;

	unsigned int vertexShader = glCreateShader(GL_VERTEX_SHADER);

	char* vertexSource = vertexFile;
	GLint fileLength = vertexFile.fileLength();
	glShaderSource(vertexShader, 1, &vertexSource, &fileLength);
	glCompileShader(vertexShader);
	glGetShaderiv(vertexShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(vertexShader, 1024, &logLength, log);
		int rax = write(1, log, logLength);
	}

	unsigned int fragmentShader = glCreateShader(GL_FRAGMENT_SHADER);

	char* fragmentSource = fragmentFile;
	fileLength = fragmentFile.fileLength();
	glShaderSource(fragmentShader, 1, &fragmentSource, &fileLength);
	glCompileShader(fragmentShader);
	glGetShaderiv(fragmentShader, GL_COMPILE_STATUS, &success);
	if (!success) {
		glGetShaderInfoLog(fragmentShader, 1024, &logLength, log);
		int rax = write(1, log, logLength);
	}

	shaderProgram = glCreateProgram();
	glAttachShader(shaderProgram, vertexShader);
	glAttachShader(shaderProgram, fragmentShader);
	glLinkProgram(shaderProgram);

	glGetProgramiv(shaderProgram, GL_LINK_STATUS, &success);
	if(!success) {
		glGetProgramInfoLog(shaderProgram, 1024, &logLength, log);
		int rax = write(1, log, logLength);
	}

	glUseProgram(shaderProgram);
	
	glDeleteShader(vertexShader);
	glDeleteShader(fragmentShader);
}

ShaderProgram::~ShaderProgram() {
	if (shaderProgram) {
		glDeleteProgram(shaderProgram);
		shaderProgram = 0;
	}
}

}

}