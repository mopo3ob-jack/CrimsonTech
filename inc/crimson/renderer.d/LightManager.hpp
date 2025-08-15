#ifndef CT_LIGHTMANAGER_HPP
#define CT_LIGHTMANAGER_HPP

#include <crimson/renderer.d/DirectionalLight.hpp>
#include <crimson/renderer.d/Shader.hpp>
#include <bitset>
#include <glad/glad.h>

namespace ct {

class LightManager {
public:
	static constexpr mstd::Size maxDirectionalLights = 32;

	LightManager(const Shader& shader);

	void update();

	DirectionalLight directionalLights[maxDirectionalLights];
	std::bitset<maxDirectionalLights> modifiedDirectionalLights;
	mstd::Size directionalCount;

private:
	GLuint directionalUniform, directionalCountUniform;
};

}

#endif
