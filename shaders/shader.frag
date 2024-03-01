#version 450 core
#extension GL_ARB_bindless_texture : enable

in Vertex {
	vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	flat uint textureID;
} vertex;

layout(std430, binding = 0) buffer ssbo1 {
	uvec2 textures[];
};

vec3 fragColor;
out vec4 outputFragColor;

void main() {
	fragColor = vertex.position;
	fragColor = texture(sampler2D(textures[0]), vertex.texCoords).rgb;

	outputFragColor = vec4(fragColor, 1.0f);
}