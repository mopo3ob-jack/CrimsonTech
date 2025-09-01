layout (location = 0) in vec3 v_position;
layout (location = 1) in vec3 v_normal;

out vec3 f_position;
out vec3 f_normal;

uniform mat4 object;
uniform mat4 rotate;
uniform mat4 camera;

void main() {
	vec4 position = object * vec4(v_position, 1.0);
	gl_Position = camera * position;
	f_position = position.xyz;
	f_normal = vec3(rotate * vec4(v_normal, 1.0));
}
