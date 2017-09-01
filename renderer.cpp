#include "renderer.hpp"

#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>

#include "util.hpp"
#include "component.hpp"

//Camera class

Camera::Camera(glm::vec3 pos, GLfloat width, GLfloat height) : pos(pos), width(width), height(height), near(0.1), far(500) {}

void Camera::pitch(float pitch) { rotate(util::toRadiansf(pitch), glm::vec3(1.0f, 0.0f, 0.0f)); }
void Camera::yaw(float yaw) { rotate(util::toRadiansf(yaw), glm::vec3(0.0f, 1.0f, 0.0f)); }
void Camera::roll(float roll) { rotate(util::toRadiansf(roll), glm::vec3(0.0f, 0.0f, 1.0f)); }
void Camera::turn(float yaw) {
	glm::quat q = glm::angleAxis(util::toRadiansf(yaw), orientation * glm::vec3(0.0f, 1.0f, 0.0f));
	rotate(q);
}

void Camera::rotate(float angleRadians, const glm::vec3& axis) {
	glm::quat q = angleAxis(angleRadians, axis);
	rotate(q);
}

void Camera::rotate(const glm::quat& rotation) {
	orientation = rotation * orientation;
}

glm::vec3 Camera::getForward() { return conjugate(orientation) * glm::vec3(0.0f, 0.0f, -1.0f); }
glm::vec3 Camera::getUp() { return conjugate(orientation) * glm::vec3(0.0f, 1.0f, 0.0f); }
glm::vec3 Camera::getRight() { return conjugate(orientation) * glm::vec3(-1.0f, 0.0f, 0.0f); }

glm::quat Camera::getQuat() {
	return orientation;
}

//Renderer class

Renderer::Renderer() : shader(Shader()) {}

void Renderer::setView(Camera camera) {
	glm::mat4 view;
	view = glm::lookAt(camera.pos, camera.pos + camera.getForward(), camera.getUp());
	shader.setMat4("view", view);
	shader.setVec3("viewPos", camera.pos);
}

void Renderer::setLighting(World world) {
	int seek = 0;
	int j = 0;
	for (int i = 0; i < world.pointLights.size(); i++) {

		Shader::PointLight light = world.pointLights[i].value;
		std::string lightStr = "pointLights[" + std::to_string(i + seek) + "]";

		while (j < world.positions.size() && world.positions[j].id < world.pointLights[i].id) {
			j++;
		}

		if (world.positions[j].id == world.pointLights[i].id) {
			glm::vec3 pos = world.positions[j].value;

			//give values
			shader.setVec3(lightStr + ".pos", pos);
			shader.setVec3(lightStr + ".ambient", light.ambient);
			shader.setVec3(lightStr + ".diffuse", light.diffuse);
			shader.setVec3(lightStr + ".specular", light.specular);
			shader.setFloat(lightStr + ".constant", 1);
			shader.setFloat(lightStr + ".linear", light.linear);
			shader.setFloat(lightStr + ".quadratic", light.quadratic);
		} else {
			seek -= 1;
			continue;
		}

		if (j == world.positions.size() - 1) {
			break;
		}
	}
	shader.setInt("pointLightCount", world.pointLights.size() + seek);
}

void Renderer::renderWorld(
	std::vector<cmpt::vao> vaos,
	std::vector<cmpt::pos> positions,
	std::vector<cmpt::orientation> orientations,
	std::vector<cmpt::scale> scales,
	std::vector<cmpt::color> colors,
	std::vector<cmpt::material> materials,
	World::Defaults defaults
) {
	auto posItr = positions.begin();
	auto orientationItr = orientations.begin();
	auto scaleItr = scales.begin();
	auto colorItr = colors.begin();
	auto materialItr = materials.begin();

	for (auto vaoItr = vaos.begin(); vaoItr != vaos.end(); ++vaoItr) {
		auto nextId = [vaoItr](const cmpt::component &c) { return c.id >= vaoItr->id; };

		posItr = std::find_if(posItr, positions.end(), nextId);
		orientationItr = std::find_if(orientationItr, orientations.end(), nextId);
		scaleItr = std::find_if(scaleItr, scales.end(), nextId);
		colorItr = std::find_if(colorItr, colors.end(), nextId);
		materialItr = std::find_if(materialItr, materials.end(), nextId);

		if (posItr == positions.end()) {
			break;
		}

		if (posItr->id > vaoItr->id) {
			continue;
		} else {
			vbd::vao vao = vaoItr->value;
			glm::vec3 pos = posItr->value;

			auto getValue = [vaoItr](auto cmptItr, auto default) { return cmptItr->id == vaoItr->id ? cmptItr->value : default; };

			//these things are needed to draw, but shouldn't be 100% neccessary.
			glm::quat orientation = getValue(orientationItr, defaults.orientation);
			glm::vec3 scale = getValue(scaleItr, defaults.scale);
			glm::vec4 color = getValue(colorItr, defaults.color);
			Shader::Material material = getValue(materialItr, defaults.material);

			glm::mat4 model;
			model = glm::translate(model, pos);
			model *= glm::toMat4(orientation);
			model = glm::scale(model, scale);

			shader.setMat4("model", model);
			shader.setVec4("color", color);
			shader.setVec3("material.ambient", material.ambient);
			shader.setVec3("material.diffuse", material.diffuse);
			shader.setVec3("material.specular", material.specular);
			shader.setFloat("material.shininess", material.shininess);

			glBindVertexArray(vao.id);
			glDrawArrays(vao.mode, 0, vao.count);
		}
	}
}

void Renderer::renderWorld(World world) {
	for (auto layer : world.layers) {
		renderWorld(layer, world.positions, world.orientations, world.scales, world.colors, world.materials, world.defaults);
	}
}

void Renderer::render(World world, Camera camera) {

	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), camera.width / camera.height, camera.near, camera.far);
	shader.setMat4("projection", projection); // might be moved outside the render function

	setView(camera);
	setLighting(world);
	renderWorld(world);

	SDL_GL_SwapWindow(window);
}

void Renderer::init(const char * vertex_file_path, const char * fragment_file_path) {
	shader.init(vertex_file_path, fragment_file_path);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);
	glEnable(GL_LINE_SMOOTH);
	glEnable(GL_BLEND);
	glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);
}