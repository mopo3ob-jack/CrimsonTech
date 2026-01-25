#version 460 core

layout (location = 0) in vec3 position;

uniform mat4 object;
uniform mat4 camera;

void main() {
	gl_Position = camera * object * vec4(position, 1.0);
}
