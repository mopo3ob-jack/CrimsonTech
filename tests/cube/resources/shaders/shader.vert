#version 460 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_color;

out vec3 f_color;

uniform mat4 object;
uniform mat4 camera;

void main() {
	gl_Position = camera * object * vec4(v_position, 1.0);
	f_color = v_color;
}
