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
	int width;
	int height;

	Camera(glm::vec3 pos, int width, int height);

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
private:
	Camera * camera;
	SDL_Window * window;
	GUI * gui;
public:
	Shader shader;

	Renderer(Shader s);

	void init();

	void setWindow(SDL_Window * w);
	void render(World world, Camera c);
};