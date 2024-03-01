#version 450 core

layout (location = 0) in vec3 position;
layout (location = 1) in vec2 texCoords;
layout (location = 2) in vec3 normal;
layout (location = 3) in vec3 tangent;
layout (location = 4) in vec3 bitangent;
layout (location = 5) in uint textureID;
layout (location = 6) in uint transformID;

out Vertex {
	vec3 position;
	vec2 texCoords;
	vec3 normal;
	vec3 tangent;
	vec3 bitangent;
	flat uint textureID;
} vertex;

uniform mat4 projection;
uniform mat4 view;

void main() {
	vertex.position = position;
	vertex.texCoords = texCoords;
	vertex.normal = normal;
	vertex.tangent = tangent;
	vertex.bitangent = bitangent;
	vertex.textureID = textureID;

	gl_Position = projection * view * vec4(position, 1.0f);
}