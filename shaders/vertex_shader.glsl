#version 330

layout(location = 0) in vec3 pos;
layout(location = 1) in vec3 normal;

out vec3 fragNormal;
out vec3 fragPos;

uniform mat4 projection;
uniform mat4 view;
uniform mat4 model;

void main() {
    gl_Position = projection * view * model * vec4(pos, 1.0f);

	fragPos = vec3(model * vec4(pos, 1.0));
	fragNormal = mat3(transpose(inverse(model))) * normal;
}