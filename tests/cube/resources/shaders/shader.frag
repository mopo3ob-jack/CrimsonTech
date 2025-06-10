#version 460 core

in vec3 f_position;
in vec3 f_normal;

out vec4 fragColor;

uniform vec3 cameraPosition;

float ambient = 0.1f;
float diffuse = 0.5f;
float specular = 0.5f;

vec3 lightColor = vec3(1.0f);

vec3 lightPosition = vec3(0.0, 1.0, 2.0);

void main() {
	vec3 lightDirection = normalize(lightPosition - f_position);
	vec3 viewDirection = normalize(f_position - cameraPosition);
	vec3 reflectDirection = reflect(lightDirection, f_normal);

	float light = ambient;
	light += diffuse * max(dot(f_normal, lightDirection), 0.0f);
	light += specular * pow(max(dot(viewDirection, reflectDirection), 0.0f), 32.0f);

	fragColor = vec4(light * lightColor, 1.0);
}
