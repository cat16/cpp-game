#pragma once

#include <glew.h>

#include <string>

class Shader {
private:
	GLuint programID;

	std::string loadFile(const char * path);
	bool compileShader(GLuint shader, std::string code);

	GLuint location(const std::string &name) const;
public:
	void init(const char * vertex_file_path, const char * fragment_file_path);

	void use();
	void setMat4(const std::string &name, const glm::mat4 &mat) const;
	void setVec4(const std::string &name, const glm::vec4 &vec) const;
	void setVec3(const std::string &name, const glm::vec3 &vec) const;
	void setFloat(const std::string &name, const float f) const;
};