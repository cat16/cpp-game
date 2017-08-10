/*
* main.cpp
*
*  Created on: Jul 23, 2017
*      Author: Bryan McShea
*/

#include <glew.h>
#include <SDL.h>
#include <glm.hpp>

#include <iostream>
#include <vector>
#include <map>
#include <chrono>

#include "world.hpp"
#include "renderer.hpp"
#include "util.hpp"
#include "input.hpp"
#include "component.hpp"
#include "VBD.hpp"
#include "shader.hpp"

void debug(const char* a) {
	std::cout << "[debug] " << a << std::endl;
}

const int width = 1000;
const int height = 800;

bool running = true;
std::chrono::duration<double> delta;
const int FPS = 60;

Renderer renderer = Renderer();

int currentId;

World world;
Camera camera(glm::vec3(0, 0, 0), width, height);
GUI gui;

GLfloat speed = 1;
GLfloat rollSpeed = 1;

InputHandler input;

int sensitivity = 100;

enum Action {
	INTERACT, CREATE, REMOVE
};
Action action = INTERACT;

GLfloat perSec(GLfloat f) {
	return f * delta.count()*50;
}

int init() {

	if (SDL_Init(SDL_INIT_VIDEO) < 0) { // Initializing SDL, on fail, throw an error
		std::cerr << "Failed to initialize SDL: " << SDL_GetError() << std::endl;
		return 1;
	}

	SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, SDL_TRUE);
	SDL_Window * window = SDL_CreateWindow(
		"Game",
		SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED, // Window position doesn't matter
		width, height,
		SDL_WINDOW_OPENGL | SDL_WINDOW_SHOWN
	);
	if (!window) {
		std::cerr << "Failed to create SDL window: " << SDL_GetError() << std::endl;
		return 1;
	}
	SDL_GL_CreateContext(window); // Create the openGl context, it handles drawing



	glewExperimental = true;
	if (GLenum err = glewInit() != GLEW_OK) {
		std::cerr << "Failed to initialize GLEW: " << glewGetErrorString(err) << std::endl;
		return 1;
	}

	renderer.init("vertex_shader.glsl", "frag_shader.glsl");

	vbd::init();



	world.generate();

	renderer.window = window;

	world.paused = true;
	SDL_SetRelativeMouseMode(SDL_FALSE);

	return 0;
}

void cleanUp() {
	world.cleanUp();
	SDL_Quit();
}

void update() {

	auto keys = input.keys;
	auto keysp = input.keysp;

	world.update();

	if (!world.paused && !keys[SDLK_LCTRL]) {
		if (keys[SDLK_w]) { camera.pos += util::move3d(glm::vec3(), camera.getForward(), -perSec(speed)); }
		if (keys[SDLK_s]) { camera.pos += util::move3d(glm::vec3(), camera.getForward(), +perSec(speed)); }

		if (keys[SDLK_d]) { camera.pos += util::move3d(glm::vec3(), camera.getRight(), +perSec(speed)); }
		if (keys[SDLK_a]) { camera.pos += util::move3d(glm::vec3(), camera.getRight(), -perSec(speed)); }

		if (keys[SDLK_SPACE]) { camera.pos += util::move3d(glm::vec3(), camera.getUp(), -perSec(speed)); }
		if (keys[SDLK_LSHIFT]) { camera.pos += util::move3d(glm::vec3(), camera.getUp(), +perSec(speed)); }

		if (keys[SDLK_q]) { camera.roll(-perSec(rollSpeed)); }
		if (keys[SDLK_e]) { camera.roll(+perSec(rollSpeed)); }

		camera.yaw(input.mousexv * sensitivity / 500000.0);
		camera.pitch(input.mouseyv * sensitivity / 500000.0);

		if (input.caps) {
			speed = 20;
		}
		else {
			speed = 5;
		}
	}

	glm::vec3 lookTo = (camera.getForward() * 15.F);
	cmpt::rigidBody lookingAt = world.raycast(-camera.pos, lookTo - camera.pos);

	if (keys[SDLK_LCTRL]) {
		if (keys[SDLK_LSHIFT]) {

			/*if (keysp[SDLK_1]) { color = glm::vec4(1, 0, 0, 1); }
			else if (keysp[SDLK_2]) { color = glm::vec4(1, 0.5, 0, 1); }
			else if (keysp[SDLK_3]) { color = glm::vec4(1, 1, 0, 1); }
			else if (keysp[SDLK_4]) { color = glm::vec4(0, 1, 0, 1); }
			else if (keysp[SDLK_5]) { color = glm::vec4(0, 1, 0.5, 1); }
			else if (keysp[SDLK_6]) { color = glm::vec4(0, 1, 1, 1); }
			else if (keysp[SDLK_7]) { color = glm::vec4(0, 0, 1, 1); }
			else if (keysp[SDLK_8]) { color = glm::vec4(0.5, 0, 1, 1); }
			else if (keysp[SDLK_9]) { color = glm::vec4(1, 0, 1, 1); }*/
		}
		else {
			/*if (keysp[SDLK_1]) {
				creating = VOXEL;
			}
			if (keysp[SDLK_2]) {
				creating = PLANET;
			}
			setProjected(lookTo);*/
		}
	}

	else {
		if (keysp[SDLK_1]) {
			action = INTERACT;
		}

		if (keys[SDLK_2]) {
			action = CREATE;
		}

		if (keysp[SDLK_3]) {
			action = REMOVE;
		}
	}

	if (action == CREATE) {
		if (cmpt::getCmpt(world.vertexBuffers, -1).size() == 0) {
			
		}
	}
	else {
		world.removeId(-1);
	}

	switch (action) {
	case INTERACT:
		/*if (mbs[SDL_BUTTON_LEFT]) {
			if (holding) {
				holding->setPos(lookTo*0.9f - camera.pos);
			}
			else {
				if (lookingAt) {
					/*if (entity::Voxel* v = dynamic_cast<entity::Voxel*>(lookingAt)) {
						holding = lookingAt;
					}
				}
			}
		}
		else {
			holding = NULL;
		}*/
		break;
	case CREATE:
		if (input.mbsp[SDL_BUTTON_LEFT]) {
			/*entity::Object * add;
			switch (creating) {
			case VOXEL:
				add = new entity::Voxel(util::btv3(lookTo-camera.pos), btQuaternion(0, 0, 0, 1), color);
				world.addObject(add);
				break;
			case PLANET:
				add = new entity::Planet(util::btv3(lookTo - camera.pos), size, size, color);
				world.addObject(add);
				break;
			}*/
		}
	case REMOVE:
		break;
	}

	if (keysp[SDLK_ESCAPE]) {
		if (SDL_GetRelativeMouseMode()) {
			world.paused = true;
			SDL_SetRelativeMouseMode(SDL_FALSE);
			SDL_WarpMouseInWindow(renderer.window, width / 2, height / 2);
		}
		else {
			world.paused = false;
			SDL_SetRelativeMouseMode(SDL_TRUE);
		}
	}
}

int main(int argc, char *argv[]) {
	if (init() > 0) {
		cleanUp();
		return 1;
	}

	while (running) {
		std::chrono::high_resolution_clock::time_point start = std::chrono::high_resolution_clock::now();


		if (!input.check())
			running = false;
		update();
		renderer.render(world, camera);

		std::chrono::high_resolution_clock::time_point finish = std::chrono::high_resolution_clock::now();
		delta = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start);
		if (1000 / FPS > (delta.count()))
			SDL_Delay(1000 / FPS - (delta.count()));

		delta = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start);
	}

	cleanUp();
	return 0;
}
