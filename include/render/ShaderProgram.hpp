#ifndef HDB_CT_SHADERPROGRAM_HPP
#define HDB_CT_SHADERPROGRAM_HPP

#include "../../glad/include/glad/glad.h"

namespace hdb {

namespace ct {

class ShaderProgram {
public:
	ShaderProgram();

	ShaderProgram(const char* vertexShaderPath, const char* fragmentShaderPath);

	~ShaderProgram();

	ShaderProgram(ShaderProgram& shader) = delete;
	ShaderProgram operator=(ShaderProgram& shader) = delete;
	void operator=(ShaderProgram shader) {
		this->shaderProgram = shader.shaderProgram;
	}

	void useProgram() const { glUseProgram(shaderProgram); }

	unsigned int getShaderProgram() const { return shaderProgram; }

protected:
	unsigned int shaderProgram = 0;
};

}

}

#endif