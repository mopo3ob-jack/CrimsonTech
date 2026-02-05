#version 460 core

layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;
layout (location = 2) in vec3 v_tangent;
layout (location = 3) in vec3 v_bitangent;
layout (location = 4) in vec2 v_uv;
layout (location = 5) in uint v_material;

out vec3 f_position;
out vec3 f_normal;
out vec3 f_tangent;
out vec3 f_bitangent;
out vec2 f_uv;
out flat uint f_material;

uniform mat4 object;
uniform mat4 rotate;
uniform mat4 camera;

void main() {
	vec4 position = object * vec4(v_position, 1.0);
	gl_Position = camera * position;
	f_position = position.xyz;
	f_normal = normalize(vec3(rotate * vec4(v_normal, 1.0)));
	f_tangent = normalize(vec3(rotate * vec4(v_tangent, 1.0)));
	f_bitangent = normalize(vec3(rotate * vec4(v_bitangent, 1.0)));
	f_uv = v_uv;
	f_material = v_material;
}
