#version 450 core

in Vertex {
	vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	flat uint textureID;
} vertex;

vec3 fragColor;
out vec4 outputFragColor;

void main() {
	fragColor = vertex.position;

	outputFragColor = vec4(fragColor, 1.0f);
}
