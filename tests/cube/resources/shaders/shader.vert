#version 460 core

layout (location = 0) in vec3 position_v;

void main() {
	gl_Position = vec4(position_v, 1.0);
}
