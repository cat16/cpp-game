#pragma once

#include <glew.h>
#include <glm.hpp>

#include <string>

class Shader {
private:
	GLuint programID;

	bool compileShader(GLuint shader, std::string code);

	GLuint location(const std::string &name) const;
public:

	//things specifically made for shaders

	struct Material {
		bool operator==(const Material& other) {
			return
				ambient == other.ambient &&
				diffuse == other.diffuse &&
				specular == other.specular &&
				shininess == other.shininess;
		};
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;
		float shininess;
		Material(
			glm::vec3 ambient,
			glm::vec3 diffuse,
			glm::vec3 specular,
			float shininess
		);
	};

	struct PointLight {
		bool operator==(const PointLight& other) {
			return
				ambient == other.ambient &&
				diffuse == other.diffuse &&
				specular == other.specular &&
				constant == other.constant &&
				linear == other.linear &&
				quadratic == other.quadratic;
		};
		glm::vec3 ambient;
		glm::vec3 diffuse;
		glm::vec3 specular;

		float constant;
		float linear;
		float quadratic;
		PointLight(
			glm::vec3 ambient,
			glm::vec3 diffuse,
			glm::vec3 specular,
			float contant,
			float linear,
			float quadradic
		);
	};

	void init(const char * vertex_file_path, const char * fragment_file_path);

	void use();

	void setMat4(const std::string &name, const glm::mat4 &mat) const;
	void setVec4(const std::string &name, const glm::vec4 &vec) const;
	void setVec3(const std::string &name, const glm::vec3 &vec) const;

	void setInt(const std::string &name, const int i);
	void setFloat(const std::string &name, const float f) const;
};