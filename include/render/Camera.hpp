#ifndef HDB_CT_CAMERA_HPP
#define HDB_CT_CAMERA_HPP

#include "../geometry/geometry.hpp"
#include "../tensor/tensor.hpp"
#include "../../glad/include/glad/glad.h"

#include <glm/glm.hpp>
#include <glm/gtc/matrix_transform.hpp>
#include <glm/gtc/type_ptr.hpp>

namespace hdb {

namespace ct {

class Camera {
public:
	Camera() {}

	Camera(unsigned int shaderProgram) {
		this->shaderProgram = shaderProgram;
		glUseProgram(shaderProgram);
		projectionUniform = glGetUniformLocation(shaderProgram, "projection");
		viewUniform = glGetUniformLocation(shaderProgram, "view");
	}

	void updateUniform(float aspectRatio, float near, float far) {
		Matrix4 projectionMatrix(0.0f);
		Matrix4 vm; //View Matrix - Abbreviated for the instantiation below
		float fovCoefficient = 1.0f / tan(fov * 0.5f);

		projectionMatrix[0][0] = fovCoefficient;
		projectionMatrix[1][1] = aspectRatio * fovCoefficient;
		projectionMatrix[2][2] = (near + far) / (far - near);
		projectionMatrix[2][3] = -2.0f * far * near / (far - near);
		projectionMatrix[3][2] = 1.0f;

		//Alpha, Beta, Gamma - Pitch, Yaw, Roll
		float cosA = cos(rotation.x), sinA = sin(rotation.x);
		float cosB = cos(rotation.y), sinB = sin(rotation.y);
		float cosY = cos(rotation.z), sinY = sin(rotation.z);

		vm[0][0] = cosY*cosB - sinY*sinA*sinB;	/**/vm[0][1] = -sinY*cosA;	/**/vm[0][2] = cosY*sinB + sinY*sinA*cosB;	/**/vm[0][3] = 0.0f;
		vm[1][0] = sinY*cosB + cosY*sinA*sinB;	/**/vm[1][1] = cosY*cosA;	/**/vm[1][2] = sinY*sinB - cosY*sinA*cosB;	/**/vm[1][3] = 0.0f;
		vm[2][0] = -cosA*sinB;					/**/vm[2][1] = sinA;		/**/vm[2][2] = cosA*cosB;					/**/vm[2][3] = 0.0f;
		vm[3][0] = 0.0f;						/**/vm[3][1] = 0.0f;		/**/vm[3][2] = 0.0f;		/**/vm[3][3] = 1.0f;

		Matrix4 translation(0.0f);

		translation[0][0] = 1.0f; translation[1][1] = 1.0f; translation[2][2] = 1.0f; translation[3][3] = 1.0f;
		
		translation[0][3] = -position.x;
		translation[1][3] = -position.y;
		translation[2][3] = -position.z;

		vm = vm * translation;

		glUniformMatrix4fv(projectionUniform, 1, GL_TRUE, (float*)projectionMatrix.data);
		glUniformMatrix4fv(viewUniform, 1, GL_TRUE, (float*)vm.data);
	}

	Vector3 position;
	Vector3 rotation;
	float fov;

	unsigned int shaderProgram;
	unsigned int projectionUniform;
	unsigned int viewUniform;
};

}

}

#endif