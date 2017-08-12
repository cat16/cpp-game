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
#include "VAO.hpp"
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
GLfloat rollSpeed = 0.5;

InputHandler input;
std::map<long, bool> activated;

int sensitivity = 100;

enum Action {
	INTERACT, CREATE, REMOVE
};
Action action = CREATE;
int entityCount = 4;

GLfloat perSec(GLfloat f) {
	return f * delta.count() * 50;
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

	world.vbs = vbd::init();



	world.generate();

	renderer.window = window;

	world.paused = true;
	SDL_SetRelativeMouseMode(SDL_FALSE);



	world.colors.push_back({-1, glm::vec4(1, 0, 0, 0.5)});
	world.vertexBuffers.push_back({ -1, world.vbs["cube"] });

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

	if (keysp[SDLK_F1]) {
		activated[SDLK_F1] = !activated[SDLK_F1];
		if (activated[SDLK_F1]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

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

		camera.zoom = input.mouseScroll;



		glm::vec3 lookTo = (camera.getForward() * 5.F);

		if (keys[SDLK_LCTRL]) {
			if (keys[SDLK_LSHIFT]) {
				if (keysp[SDLK_1])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(1, 0, 0, 0.5) });
				if (keysp[SDLK_2])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(1, 0.5, 0, 0.5) });
				if (keysp[SDLK_3])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(1, 1, 0, 0.5) });
				if (keysp[SDLK_4])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(0, 1, 0, 0.5) });
				if (keysp[SDLK_5])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(0, 1, 0.5, 0.5) });
				if (keysp[SDLK_6])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(0, 1, 1, 0.5) });
				if (keysp[SDLK_7])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(0, 0, 1, 0.5) });
				if (keysp[SDLK_8])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(0.5, 0, 1, 0.5) });
				if (keysp[SDLK_9])
					cmpt::updateCmpt(world.colors, { -1, glm::vec4(1, 0, 1, 0.5) });
			}
			else {
				if (keysp[SDLK_1]) {
					cmpt::removeCmpt(world.vertexBuffers, -1);
					world.vertexBuffers.push_back({ -1, world.vbs["cube"] });
				}
				else if (keysp[SDLK_2]) {
					cmpt::removeCmpt(world.vertexBuffers, -1);
				}
			}
		}
		else {
			if (keysp[SDLK_1]) {
				action = INTERACT;
			}
			else if (keysp[SDLK_2]) {
				action = CREATE;
			}
			else if (keysp[SDLK_3]) {
				action = REMOVE;
			}
		}

		if (action == CREATE) {
			cmpt::updateCmpt(world.positions, cmpt::pos({ -1, lookTo + camera.pos }));
			cmpt::updateCmpt(world.orientations, cmpt::orientation({ -1, glm::inverse(camera.getQuat()) }));
			if (input.mbsp[SDL_BUTTON_LEFT]) {
				glm::vec4 color = cmpt::getCmptSafe(world.colors, new cmpt::color(-1, world.defaults.color))->value;
				color.w = 1;
				world.createRigidBox(entityCount++,
					cmpt::getCmpt(world.positions, -1)->value,
					cmpt::getCmpt(world.orientations, -1)->value,
					cmpt::getCmptSafe(world.scales, new cmpt::scale(-1, world.defaults.scale))->value,
					color,
					1
				);
			}
		}
		else {
			cmpt::removeCmpt(world.positions, -1);
		}

		if (action == INTERACT) {
			cmpt::rigidBody * lookingAt = world.raycast(camera.pos, lookTo + camera.pos);
			if (lookingAt != NULL && input.mbs[SDL_BUTTON_LEFT]) {
				lookingAt->setPos(lookTo + camera.pos);
			}
		}
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
