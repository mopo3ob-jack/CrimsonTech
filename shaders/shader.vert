#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in lowp vec4 color;
layout (location = 3) in vec3 normal;
layout (location = 4) in vec3 tangent;
layout (location = 5) in vec3 bitangent;

struct Vertex {
	vec3 position;
	vec2 texCoords;
	vec4 color;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
};

out Vertex inVertex;

uniform mat4 projection;
uniform mat4 view;

void main() {
	inVertex.position = position;
	inVertex.color = color / 255.0f;
	inVertex.texCoords = texCoords;
	inVertex.normal = normal;
	inVertex.tangent = tangent;
	inVertex.bitangent = bitangent;

	gl_Position = projection * view * vec4(position, 1.0f);
}