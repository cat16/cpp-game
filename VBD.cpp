#include "VBD.hpp"

#include <vector>

void vbd::init() {

	static const GLfloat cubearr[] = {
		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,

		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f, -0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,

		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,

		-0.5f, -0.5f, -0.5f,
		0.5f, -0.5f, -0.5f,
		0.5f, -0.5f,  0.5f,
		0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f,  0.5f,
		-0.5f, -0.5f, -0.5f,

		-0.5f,  0.5f, -0.5f,
		0.5f,  0.5f, -0.5f,
		0.5f,  0.5f,  0.5f,
		0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f,  0.5f,
		-0.5f,  0.5f, -0.5f
	};
	std::vector<GLfloat> cubebd(cubearr, cubearr + sizeof(cubearr) / sizeof(cubearr[0]));

	cube = createVBO(cubebd);

	sphere = cube;
	/*glGenVertexArrays(1, &sphere);
	glBindVertexArray(sphere);
	std::vector<GLfloat> ballVerts;
	for (int i = 0; i <= 40; i++)
	{
		double lat0 = util::PI * (-0.5 + (double)(i - 1) / 40);
		double z0 = sin(lat0);
		double zr0 = cos(lat0);

		double lat1 = util::PI * (-0.5 + (double)i / 40);
		double z1 = sin(lat1);
		double zr1 = cos(lat1);

		for (int j = 0; j <= 40; j++)
		{
			double lng = 2 * util::PI * (double)(j - 1) / 40;
			double x = cos(lng);
			double y = sin(lng);

			//x, y, z
			ballVerts.push_back(x * zr0);
			ballVerts.push_back(y * zr0);
			ballVerts.push_back(z0);

			//r, g, b, a
			ballVerts.push_back(0.0f);
			ballVerts.push_back(1.0f);
			ballVerts.push_back(0.0f);
			ballVerts.push_back(1.0f);

			ballVerts.push_back(x * zr1);
			ballVerts.push_back(y * zr1);
			ballVerts.push_back(z1);

			ballVerts.push_back(0.0f);
			ballVerts.push_back(1.0f);
			ballVerts.push_back(0.0f);
			ballVerts.push_back(1.0f);
		}
	}
	glBufferData(GL_VERTEX_ARRAY, sizeof(ballVerts.data()), ballVerts.data(), GL_STATIC_DRAW);*/
}

GLuint vbd::createVBO(const std::vector<GLfloat>& data) {
	int dimensions = 3;

	GLuint vbo;
	glGenBuffers(1, &vbo);
	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER,
		data.size() * sizeof(GLfloat),
		data.data(),
		GL_STATIC_DRAW);

	glVertexAttribPointer(count,
		dimensions,
		GL_FLOAT,
		GL_FALSE,
		0,
		(GLvoid*)0);

	glEnableVertexAttribArray(count++);

	return vbo;
}