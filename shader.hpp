#pragma once

#include <glew.h>

#include <string>

class Shader {
private:
	GLuint programID;
	GLuint matrixID;

	std::string loadFile(const char * path);
	void compileShader(GLuint shader, std::string code);
public:
	void init(const char * vertex_file_path, const char * fragment_file_path);
	GLuint getID();
	GLuint getMatID();

	void setMat4(const std::string &name, const glm::mat4 &mat) const;
};