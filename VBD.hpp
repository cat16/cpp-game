#pragma once

#include <glew.h>

#include <vector>

#include "util.hpp"

namespace vbd {
	static int count;

	static GLuint cube;
	static GLuint sphere;

	void init();
	GLuint createVBO(const std::vector<GLfloat> &data);
}