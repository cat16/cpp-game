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

enum Layer {
	BASE = 0,
	TRANSPARENT = 1
};

GLfloat speed = 1;
GLfloat rollSpeed = 0.5;

InputHandler input;
std::map<long, bool> activated;

int sensitivity = 100;

enum Action {
	INTERACT, CREATE, REMOVE
};
Action action = INTERACT;
int projectionId = 0;
cmpt::rigidBody * holding = NULL;
btCollisionShape * shape = NULL;
btScalar gravity = NULL;
btScalar mass = 1;
btScalar size = 1;
int entityCount = 2;

GLfloat perSec(GLfloat f) {
	return f * delta.count();
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

	renderer.init("shaders/vertex_shader.glsl", "shaders/frag_shader.glsl");

	world.vbs = vbd::init();



	for (int i = 0; i < 2; i++) {
		world.layers.push_back(std::vector<cmpt::vao>());
	}

	int id;
	//star
	id = 1;
	cmpt::addCmpt(world.positions, { id, glm::vec3(0, 50, 0) });
	cmpt::addCmpt(world.pointLights, { id, Shader::PointLight(
		glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1),
		1.0f, 0.007f, 0.0002f
	) });
	world.createRigidSphere(id, glm::vec3(0, 50, 0), glm::quat(1, 1, 1, 1), btScalar(1), glm::vec4(1, 1, 1, 1), BASE, 0);
	cmpt::addCmpt(world.materials, { id, Shader::Material(glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), glm::vec3(1, 1, 1), 1) });
	//planet
	id = 2;
	cmpt::addCmpt(world.gravities, { id, 100 });
	world.createRigidSphere(id, glm::vec3(0, -51, 0), glm::quat(1, 1, 1, 1), btScalar(50), glm::vec4(0.5, 0.5, 0.5, 1), BASE, 0);
	cmpt::addCmpt(world.materials, {id, Shader::Material(glm::vec3(0.1, 0.1, 0.1), glm::vec3(1, 1, 1), glm::vec3(0, 0, 0), 1)});

	renderer.window = window;

	world.paused = true;
	SDL_SetRelativeMouseMode(SDL_FALSE);



	input.mouseScroll = 5;
	shape = new btBoxShape(util::btv3(cmpt::getCmptSafe(world.scales, new cmpt::scale(projectionId, world.defaults.scale))->value / 2.f));
	cmpt::addCmpt(world.colors,{ projectionId, glm::vec4(1, 0, 0, 0.5) });
	cmpt::addCmpt(world.layers[TRANSPARENT], { projectionId, world.vbs["cube"] });

	return 0;
}

void cleanUp() {
	world.cleanUp();
	SDL_Quit();
}

void updateColor(glm::vec3 color) {
	cmpt::updateCmpt(world.colors, { projectionId, glm::vec4(color.x, color.y, color.z, 0.5) });
	if (cmpt::getCmpt(world.pointLights, projectionId)) {
		cmpt::getCmpt(world.pointLights, projectionId)->value.ambient = color;
		cmpt::getCmpt(world.pointLights, projectionId)->value.diffuse = color;
		cmpt::getCmpt(world.pointLights, projectionId)->value.specular = color;
	}
}

void resetCreate() {
	cmpt::removeCmpt(world.pointLights, projectionId);
	cmpt::removeCmpt(world.materials, projectionId);
	cmpt::removeCmpt(world.gravities, projectionId);
	mass = 1;
	gravity = 0;
	shape = NULL;
}

void update() {

	auto keys = input.keys;
	auto keysp = input.keysp;

	world.update(delta.count());

	if (keysp[SDLK_F1]) {
		activated[SDLK_F1] = !activated[SDLK_F1];
		if (activated[SDLK_F1]) {
			glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
		}
		else {
			glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
		}
	}

	if (!world.paused) {

		if (!keys[SDLK_LCTRL]) {

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



		if (input.mouseScroll < 1) input.mouseScroll = 1;
		GLfloat reach = input.mouseScroll;

		glm::vec3 lookTo = (camera.getForward() * reach);

		if (keys[SDLK_LCTRL]) {
			if (keys[SDLK_LSHIFT]) {
				if (keysp[SDLK_1])
					updateColor(glm::vec4(1, 0, 0, 0.5));
				if (keysp[SDLK_2])
					updateColor(glm::vec4(1, 0.5, 0, 0.5));
				if (keysp[SDLK_3])
					updateColor(glm::vec4(1, 1, 0, 0.5));
				if (keysp[SDLK_4])
					updateColor(glm::vec4(0, 1, 0, 0.5));
				if (keysp[SDLK_5])
					updateColor(glm::vec4(0, 1, 0.5, 0.5));
				if (keysp[SDLK_6])
					updateColor(glm::vec4(0, 1, 1, 0.5));
				if (keysp[SDLK_7])
					updateColor(glm::vec4(0, 0, 1, 0.5));
				if (keysp[SDLK_8])
					updateColor(glm::vec4(0.5, 0, 1, 0.5));
				if (keysp[SDLK_9])
					updateColor(glm::vec4(1, 0, 1, 0.5));
			}
			else {
				if (keysp[SDLK_1]) {
					resetCreate();

					cmpt::updateCmpt(world.layers[TRANSPARENT], { projectionId, world.vbs["cube"] });
					cmpt::updateCmpt(world.scales, cmpt::scale(projectionId, glm::vec3(1, 1, 1)));
					shape = new btBoxShape(util::btv3(cmpt::getCmptSafe(world.scales, new cmpt::scale(projectionId, world.defaults.scale))->value / 2.f));
				}
				else if (keysp[SDLK_2]) {
					resetCreate();

					cmpt::updateCmpt(world.layers[TRANSPARENT], { projectionId, world.vbs["sphere"] });
					cmpt::updateCmpt(world.scales, cmpt::scale(projectionId, glm::vec3(0.5, 0.5, 0.5)));
					shape = new btSphereShape(btScalar(cmpt::getCmptSafe(world.scales, new cmpt::scale(projectionId, world.defaults.scale))->value.x));
				}
				else if (keysp[SDLK_3]) {
					resetCreate();

					mass = 0;
					gravity = 10;
					cmpt::updateCmpt(world.materials, { projectionId, Shader::Material(
						glm::vec3(0.3, 0.3, 0.3),
						glm::vec3(1, 1, 1),
						glm::vec3(0, 0, 0),
						1
					) });
					cmpt::updateCmpt(world.layers[TRANSPARENT], { projectionId, world.vbs["sphere"] });
					cmpt::updateCmpt(world.scales, cmpt::scale(projectionId, glm::vec3(10, 10, 10)));
					shape = new btSphereShape(btScalar(cmpt::getCmptSafe(world.scales, new cmpt::scale(projectionId, world.defaults.scale))->value.x));
				}
				else if (keysp[SDLK_4]) {
					resetCreate();

					cmpt::updateCmpt(world.materials, { projectionId, Shader::Material(
						glm::vec3(1, 1, 1),
						glm::vec3(1, 1, 1),
						glm::vec3(1, 1, 1),
						1
					) });
					glm::vec3 color = glm::vec3(cmpt::getCmptSafe(world.colors, new cmpt::color(projectionId, world.defaults.color))->value);
					cmpt::updateCmpt(world.layers[TRANSPARENT], { projectionId, world.vbs["sphere"] });
					cmpt::updateCmpt(world.pointLights, { projectionId, Shader::PointLight(
						color, color, color,
						1.0f, 0.007f, 0.0002f
					) });
					cmpt::updateCmpt(world.scales, cmpt::scale(projectionId, glm::vec3(0.5, 0.5, 0.5)));
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
			cmpt::updateCmpt(world.positions, cmpt::pos({ projectionId, lookTo + camera.pos }));
			cmpt::updateCmpt(world.orientations, cmpt::orientation({ projectionId, glm::inverse(camera.getQuat()) }));
			if (input.mbsp[SDL_BUTTON_LEFT]) {
				glm::vec4 color = cmpt::getCmptSafe(world.colors, new cmpt::color(projectionId, world.defaults.color))->value;
				color.w = 1;

				entityCount++;
				cmpt::copyCmpt(world.positions, projectionId, entityCount);
				cmpt::copyCmpt(world.orientations, projectionId, entityCount);
				cmpt::copyCmpt(world.scales, projectionId, entityCount);
				cmpt::copyCmpt(world.layers[TRANSPARENT], world.layers[BASE], projectionId, entityCount);
				cmpt::copyCmpt(world.materials, projectionId, entityCount);
				cmpt::copyCmpt(world.pointLights, projectionId, entityCount);
				cmpt::copyCmpt(world.gravities, projectionId, entityCount);
				cmpt::addCmpt(world.colors, { entityCount, color });
				if (shape)
					world.addRigidBody(cmpt::rigidBody(entityCount, camera.pos + lookTo, camera.getQuat(), shape, mass));
				if (gravity)
					cmpt::addCmpt(world.gravities,{ entityCount, gravity });
			}
		}
		else {
			cmpt::removeCmpt(world.positions, projectionId);
		}

		if (action == INTERACT) {
			cmpt::rigidBody * lookingAt = world.raycast(camera.pos, lookTo + camera.pos);
			if (lookingAt && input.mbsp[SDL_BUTTON_LEFT]) {
				holding = lookingAt;
			}
			if (!input.mbs[SDL_BUTTON_LEFT]) {
				holding = NULL;
			}
			if (holding) {
				holding->setPos(lookTo + camera.pos);
				holding->value->activate();
			}

			if (lookingAt && input.mbs[SDL_BUTTON_RIGHT]) {
				lookingAt->value->applyForce(util::btv3(camera.getForward()*25.f), btVector3(0, 0, 0));
				lookingAt->value->activate();
			}
		}
		else {
			holding = NULL;
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

	std::chrono::high_resolution_clock::time_point start, finish;

	while (running) {
		start = std::chrono::high_resolution_clock::now();


		if (!input.check())
			running = false;
		update();
		renderer.render(world, camera);

		finish = std::chrono::high_resolution_clock::now();
		delta = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start);
		if (1000 / FPS > (delta.count()))
			SDL_Delay(1000 / FPS - (delta.count()));
		finish = std::chrono::high_resolution_clock::now();
		delta = std::chrono::duration_cast<std::chrono::duration<double>>(finish - start);
	}

	cleanUp();
	return 0;
}
