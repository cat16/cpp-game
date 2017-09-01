#pragma once

#include <glew.h>
#include <SDL.h>

#include "world.hpp"
#include "gui.hpp"
#include "shader.hpp"

class Camera {
private:
	glm::quat orientation;
public:
	glm::vec3 pos;
	GLfloat width, height, near, far;	
	GLfloat zoom;

	Camera(glm::vec3 pos, GLfloat width, GLfloat height);

	void pitch(float pitch);
	void yaw(float yaw);
	void roll(float roll);
	void turn(float turnRadians);

	void rotate(float angleRadians, const glm::vec3& axis);
	void rotate(const glm::quat& rotation);

	glm::vec3 getForward();
	glm::vec3 getUp();
	glm::vec3 getRight();

	glm::quat getQuat();
};

class Renderer {
public:

	Shader shader;

	SDL_Window * window;

	Renderer();

	void init(const char * vertex_file_path, const char * fragment_file_path);

	void setView(Camera camera);
	void setLighting(World world);
	void renderWorld(World world);
	void renderWorld(
		std::vector<cmpt::vao> vaos,
		std::vector<cmpt::pos> positions,
		std::vector<cmpt::orientation> orientations,
		std::vector<cmpt::scale> scales,
		std::vector<cmpt::color> colors,
		std::vector<cmpt::material> materials,
		World::Defaults defaults
	);

	void render(World world, Camera c);
};