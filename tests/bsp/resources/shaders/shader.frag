#version 460 core

in vec3 f_position;
in vec3 f_normal;

out vec4 fragColor;

uniform vec3 cameraPosition;
uniform int wireframe;

float ambient = 0.3f;
float diffuse = 0.7f;
float specular = 0.2f;

vec3 lightColor = vec3(0.0f, 1.0f, 0.0f);

vec3 lightPosition = vec3(0.0, 1.0, 1.0);

void main() {
	lightColor = abs(f_normal);

	vec3 lightDirection = normalize(lightPosition - f_position);
	vec3 viewDirection = normalize(f_position - cameraPosition);
	vec3 reflectDirection = reflect(lightDirection, f_normal);

	float light;
	if (wireframe == 1) {
		light = 1.0f;
	} else {
		light = ambient;
		light += diffuse * max(dot(f_normal, lightDirection), 0.0f);
		light += specular * pow(max(dot(viewDirection, reflectDirection), 0.0f), 32.0f);

		light *= 1.0f / (1.0f + 0.1 * distance(f_position, lightPosition));
	}

	fragColor = vec4(lightColor * light, 1.0);
}
