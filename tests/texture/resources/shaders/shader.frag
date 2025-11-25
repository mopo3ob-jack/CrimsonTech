#version 460 core

in vec2 f_uv;

out vec4 fragColor;

void main() {
	fragColor = vec4(f_uv, 1.0, 1.0);
}
