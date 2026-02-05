#version 460 core

in vec3 f_position;
in vec3 f_normal;
in vec3 f_tangent;
in vec3 f_bitangent;
in vec2 f_uv;
in flat uint f_material;

out vec4 fragColor;

uniform sampler2DArray textures[10];

uniform vec3 cameraPosition;

struct Material {
	uint albedoIndex, albedoUnit;
	uint normalIndex, normalUnit;
};

layout (binding = 0, std430) readonly buffer mBuffer {
	Material materials[];
};

float ambient = 0.1f;
float diffuse = 0.7f;
float specular = 0.6f;

vec3 lightColor = vec3(1.0f, 1.0f, 1.0f);

vec3 lightPosition = vec3(0.0, 0.0, 1.0);

void main() {
	vec3 albedo = texture(
		textures[materials[f_material].albedoUnit],
		vec3(f_uv, materials[f_material].albedoIndex)
	).xyz;

	vec3 bump = texture(
		textures[materials[f_material].normalUnit],
		vec3(f_uv, materials[f_material].normalIndex)
	).xyz;

	bump = bump * 2 - vec3(1);

	mat3 bumpMatrix;
	bumpMatrix[0] = f_tangent;
	bumpMatrix[1] = f_bitangent;
	bumpMatrix[2] = f_normal;

	vec3 normal = bumpMatrix * bump;
	normal = normalize(normal);

	vec3 lightDirection = normalize(lightPosition - f_position);
	vec3 viewDirection = normalize(f_position - cameraPosition);
	vec3 reflectDirection = reflect(lightDirection, normal);
    
	float light;
	light = ambient;
	light += diffuse * max(dot(normal, lightDirection), 0.0f);
	light += specular * pow(max(dot(viewDirection, reflectDirection), 0.0f), 32.0f);
    
	light *= 1.0f / (1.0f + 0.1 * distance(f_position, lightPosition));
	//light = min(2.0, light);

	fragColor = vec4(light * lightColor * albedo, 1.0);
	//fragColor.xyz /= fragColor.xyz + vec3(1.0f);
}
