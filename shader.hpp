#pragma once

#include <Windows.h>

#include <glew.h>

class Shader {
private:
	GLuint programID;
	GLuint matrixID;
public:
	void init(const char * vertex_file_path, const char * fragment_file_path);
	GLuint getID();
	GLuint getMatID();

	void setMat4(const std::string &name, const glm::mat4 &mat) const;
};