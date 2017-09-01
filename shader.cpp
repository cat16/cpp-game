#include <glm.hpp>

#include <fstream>
#include <vector>
#include <iostream>

#include "shader.hpp"
#include "util.hpp"

Shader::Material::Material(
	glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular,
	float shininess
) : ambient(ambient), diffuse(diffuse), specular(specular), shininess(shininess) {}

Shader::PointLight::PointLight(
	glm::vec3 ambient,
	glm::vec3 diffuse,
	glm::vec3 specular,
	float contant,
	float linear,
	float quadradic
) : ambient(ambient), diffuse(diffuse), specular(specular), constant(constant), linear(linear), quadratic(quadradic) {}

void Shader::init(const char * vertex_file_path, const char * fragment_file_path) {
	// Create the shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexShaderCode = util::loadFile(vertex_file_path);
	std::string fragShaderCode = util::loadFile(fragment_file_path);

	int compiled = 0;

	if (compileShader(vertexShader, vertexShaderCode)) compiled++;
	if (compileShader(fragShader, fragShaderCode)) compiled++;

	// Link the program
	programID = glCreateProgram();



	glAttachShader(programID, vertexShader);
	glAttachShader(programID, fragShader);
	glLinkProgram(programID);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Check the program
	glGetProgramiv(programID, GL_LINK_STATUS, &Result);
	glGetProgramiv(programID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(programID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cout << "Could not link shaders: " << &ProgramErrorMessage[0] << std::endl;
	}
	else {
		std::cout << "Successfully compiled and linked " << compiled << "/2 shaders" << std::endl;
	}

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);
}

bool Shader::compileShader(GLuint shader, std::string code) {
	GLint result = GL_FALSE;
	int infoLogLength;

	//compile
	char const * codeptr = code.c_str();
	glShaderSource(shader, 1, &codeptr, NULL);
	glCompileShader(shader);

	//check for errors
	glGetShaderiv(shader, GL_COMPILE_STATUS, &result);
	glGetShaderiv(shader, GL_INFO_LOG_LENGTH, &infoLogLength);
	if (infoLogLength > 0) {
		std::vector<char> errorMessage(infoLogLength + 1);
		glGetShaderInfoLog(shader, infoLogLength, NULL, &errorMessage[0]);
		std::cout << "Could not compile shader " << shader << ":" << &errorMessage[0] << std::endl;
		return false;
	}
	return true;
}

GLuint Shader::location(const std::string &name) const {
	return glGetUniformLocation(programID, name.c_str());
}

void Shader::use() {
	glUseProgram(programID);
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
	glUniformMatrix4fv(location(name), 1, GL_FALSE, &mat[0][0]);
}

void Shader::setVec4(const std::string &name, const glm::vec4 &vec) const {
	glUniform4f(location(name), vec.x, vec.y, vec.z, vec.w);
}

void Shader::setVec3(const std::string &name, const glm::vec3 &vec) const {
	glUniform3f(location(name), vec.x, vec.y, vec.z);
}

void Shader::setInt(const std::string &name, const int i) {
	glUniform1i(location(name), i);
}

void Shader::setFloat(const std::string &name, const float f) const {
	glUniform1f(location(name), f);
}