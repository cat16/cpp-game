#include <glm.hpp>
#include <gtc/type_ptr.hpp>

#include <fstream>
#include <vector>
#include <iostream>

#include "shader.hpp"

void Shader::init(const char * vertex_file_path, const char * fragment_file_path) {
	// Create the shaders
	GLuint vertexShader = glCreateShader(GL_VERTEX_SHADER);
	GLuint fragShader = glCreateShader(GL_FRAGMENT_SHADER);

	std::string vertexShaderCode = loadFile(vertex_file_path);
	std::string fragShaderCode = loadFile(fragment_file_path);

	int compiled = 0;

	if (compileShader(vertexShader, vertexShaderCode)) compiled++;
	if (compileShader(fragShader, fragShaderCode)) compiled++;

	// Link the program
	GLuint ProgramID = glCreateProgram();
	glAttachShader(ProgramID, vertexShader);
	glAttachShader(ProgramID, fragShader);
	glLinkProgram(ProgramID);

	GLint Result = GL_FALSE;
	int InfoLogLength;

	// Check the program
	glGetProgramiv(ProgramID, GL_LINK_STATUS, &Result);
	glGetProgramiv(ProgramID, GL_INFO_LOG_LENGTH, &InfoLogLength);
	if (InfoLogLength > 0) {
		std::vector<char> ProgramErrorMessage(InfoLogLength + 1);
		glGetProgramInfoLog(ProgramID, InfoLogLength, NULL, &ProgramErrorMessage[0]);
		std::cout << "Could not link shaders: " << &ProgramErrorMessage[0] << std::endl;
	}
	else {
		std::cout << "Successfully compiled and linked " << compiled << "/2 shaders" << std::endl;
	}


	//glDetachShader(ProgramID, vertexShader);
	//glDetachShader(ProgramID, fragShader);

	glDeleteShader(vertexShader);
	glDeleteShader(fragShader);

	programID = ProgramID;
}

GLuint Shader::getID() {
	return programID;
}

void Shader::setMat4(const std::string &name, const glm::mat4 &mat) const {
	unsigned int location = glGetUniformLocation(programID, name.c_str());
	glUniformMatrix4fv(location, 1, GL_FALSE, glm::value_ptr(mat));
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

std::string Shader::loadFile(const char * path) {
	std::string text;
	std::ifstream fileStream(path, std::ios::in);
	if (fileStream.is_open()) {
		std::string line = "";
		while (getline(fileStream, line))
			text += "\n" + line;
		fileStream.close();
	}
	else {
		std::cerr << "Could not open file " << path << std::endl;
		getchar();
	}

	return text;
}