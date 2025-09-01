struct DirectionalLight {
	vec3 position, color;
	float diffuse, specular;
};

struct PointLight {
	vec3 position, color;
	float diffuse, specular;
	float linear, quadratic;
};

struct SpotLight {
	vec3 position, color;
	float diffuse, specular;
	vec3 direction;
	float linear, quadratic;
	float innerRadius, outerRadius;
};

uniform vec3 ambientLight;

uniform DirectionalLight directionalLights[32];
uniform PointLight pointLights[32];
uniform PointLight spotLights[32];

uniform int directionalCount;
uniform int pointCount;
uniform int spotCount;

vec3 calculateDirectional(vec3 position, vec3 normal, vec3 cameraPosition) {
	vec3 result = vec3(0.0f);

	vec3 direction = normalize(position - cameraPosition);

	for (int i = 0; i < directionalCount; ++i) {
		vec3 reflection = reflect(directionalLights[i].position, normal);
		float light = 0.0f;
		light += directionalLights[i].diffuse * max(dot(normal, directionalLights[i].position), 0.0f);
		light += directionalLights[i].specular * pow(max(dot(direction, reflection), 0.0f), 32.0f);
		result += directionalLights[i].color * light;
	}

	return result;
}

vec3 calculateLighting(vec3 position, vec3 normal, vec3 cameraPosition) {
	return
		ambientLight + 
		calculateDirectional(position, normal, cameraPosition)
	;
}
