#version 330 core

in vec3 fragPos;
in vec3 fragNormal;

out vec4 fragColor;

struct Material {
	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float shininess;
};

struct PointLight {
	vec3 pos;

	vec3 ambient;
	vec3 diffuse;
	vec3 specular;

	float constant;
	float linear;
	float quadratic;
};

uniform vec3 viewPos;
uniform vec4 color;

uniform Material material;
uniform int pointLightCount;
uniform PointLight pointLights[100];

vec3 calcPointLight(PointLight light, vec3 normal, vec3 fragPos, vec3 viewDir)
{

	vec3 lightDir = normalize(light.pos - fragPos);
	float diff = max(dot(normal, lightDir), 0.0);

	vec3 reflectDir = reflect(-lightDir, normal);
	float spec = pow(max(dot(viewDir, reflectDir), 0.0), material.shininess);

	vec3 ambient = light.ambient * material.ambient;
	vec3 diffuse = light.diffuse * diff * material.diffuse;
	vec3 specular = light.specular * spec * material.specular;

	float distance = length(light.pos - fragPos);
	float attenuation = 1.0 / (light.constant + light.linear * distance + light.quadratic * (distance * distance));

	ambient *= attenuation;
	diffuse *= attenuation;
	specular *= attenuation;

	return vec3(ambient + diffuse + specular);
}

void main() {
	vec3 norm = normalize(fragNormal);
	vec3 viewDir = normalize(viewPos - fragPos);

	vec3 lighting = vec3(0, 0, 0);

	for (int i = 0; i < pointLightCount; i++) {
		lighting += calcPointLight(pointLights[i], norm, fragPos, viewDir);
	}

    fragColor = color * vec4(lighting, 1);
}