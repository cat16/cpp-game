#pragma once

#include <glew.h>

#include <string>

class Shader {
private:
	GLuint programID;

	std::string loadFile(const char * path);
	bool compileShader(GLuint shader, std::string code);
public:
	void init(const char * vertex_file_path, const char * fragment_file_path);
	GLuint getID();

	void setMat4(const std::string &name, const glm::mat4 &mat) const;
	void setVec4(const std::string &name, const glm::vec4 &vec) const;
};