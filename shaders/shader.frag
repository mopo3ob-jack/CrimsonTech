#version 450 core

struct Vertex {
	vec3 position;
	vec2 texCoords;
	vec4 color;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
};

in Vertex inVertex;

vec3 fragColor;
out vec4 outputFragColor;

void main() {
	fragColor = inVertex.color.xyz;

	outputFragColor = vec4(fragColor, 1.0f);
}