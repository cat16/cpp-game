#pragma once

#include <glew.h>

#include <vector>
#include <map>

#include "util.hpp"

namespace vbd {
	struct vao {
		GLuint id;
		GLenum mode;
		GLsizei count;

		vao(GLuint id, GLenum mode, GLsizei count);
		vao();
	};

	static int count;

	std::map<std::string, vao> init();
	GLuint createVAO(const std::vector<GLfloat> &data, int dimensions);
}