#include <crimson/renderer.d/LightManager.hpp>

namespace ct {

LightManager::LightManager(const Shader& shader) {
	directionalUniform = glGetUniformLocation(shader, "directionalLights");
	directionalCountUniform = glGetUniformLocation(shader, "directionalCount");
}

void LightManager::update() {
	using namespace mstd;

	for (Size i = 0; i < directionalCount; ++i) {
		if (modifiedDirectionalLights[i]) {

		}
	}
}

}
