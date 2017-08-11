#include "VAO.hpp"

#include <vector>

vbd::vao::vao(GLuint id, GLenum mode, GLsizei count) : id(id), mode(mode), count(count) {}
vbd::vao::vao() {}

std::map<std::string, vbd::vao> vbd::init() {
	std::map<std::string, vao> data;

	const GLfloat cubearr[] = {
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
	data["cube"] = vao(createVAO(cubebd, 3), GL_TRIANGLES, 36);


	std::vector<GLfloat> spherebd;
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
			spherebd.push_back(x * zr0);
			spherebd.push_back(y * zr0);
			spherebd.push_back(z0);

			spherebd.push_back(x * zr1);
			spherebd.push_back(y * zr1);
			spherebd.push_back(z1);
		}
	}
	data["sphere"] = vao(createVAO(spherebd, 3), GL_TRIANGLE_STRIP, 3200*2);

	return data;
}

GLuint vbd::createVAO(const std::vector<GLfloat>& data, int dimensions) {
	GLuint vbo, vao;

	glGenVertexArrays(1, &vao);
	glGenBuffers(1, &vbo);

	glBindVertexArray(vao);

	glBindBuffer(GL_ARRAY_BUFFER, vbo);
	glBufferData(GL_ARRAY_BUFFER, data.size() * sizeof(GLfloat), data.data(), GL_STATIC_DRAW);

	glVertexAttribPointer(0, dimensions, GL_FLOAT, GL_FALSE, 3 * sizeof(float), (GLvoid*)0);
	glEnableVertexAttribArray(0);

	glBindBuffer(GL_ARRAY_BUFFER, 0);
	glBindVertexArray(0);

	return vao;
}