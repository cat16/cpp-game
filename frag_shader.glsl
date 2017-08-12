#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

out vec4 FragColor;

uniform vec4 color;
uniform vec3 lightColor;
uniform vec3 lightPos;
uniform float lightAmbient;

void main() {
	vec3 norm = normalize(fragNormal);
	vec3 lightDir = normalize(lightPos - fragPos);
	float diff = max(dot(norm, lightDir), 0.0);
	vec3 lightDiffuse = diff * lightColor;

	vec4 lighting = vec4(lightColor*(lightAmbient+lightDiffuse), 1);
    FragColor = color * lighting;
}