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

void Renderer::render(World world, Camera camera) {

	glClearColor(0.f, 0.f, 0.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	shader.use();

	glm::mat4 view;
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), camera.width / camera.height, camera.near, camera.far);
	view = glm::lookAt(camera.pos, camera.pos + camera.getForward(), camera.getUp());

	shader.setMat4("projection", projection); // might be moved outside the render function
	shader.setMat4("view", view);

	shader.setVec3("lightColor", glm::vec3(1, 1, 1));
	shader.setVec3("lightPos", glm::vec3(0, 0, 0));
	shader.setFloat("lightAmbient", 0.1);

	for (cmpt::vao vb : world.vertexBuffers) {

		if (!cmpt::getCmpt(world.positions, vb.id))
			continue;

		glm::vec3 pos = cmpt::getCmpt(world.positions, vb.id)->value;
		//these things are needed to draw, but shouldn't be 100% neccessary.
		glm::quat orientation = cmpt::getCmptSafe(world.orientations, new cmpt::orientation( vb.id, world.defaults.orientation ))->value;
		glm::vec3 scale = cmpt::getCmptSafe(world.scales, new cmpt::scale(vb.id, world.defaults.scale))->value;
		glm::vec4 color = cmpt::getCmptSafe(world.colors, new cmpt::color(vb.id, world.defaults.color))->value;

		if (!cmpt::getCmpt(world.orientations, vb.id))
			orientation = glm::quat(0, 0, 0, 1);
		else
			orientation = cmpt::getCmpt(world.orientations, vb.id)->value;

		if (!cmpt::getCmpt(world.colors, vb.id))
			color = glm::vec4(0.5, 0.5, 0.5, 1);
		else
			color = cmpt::getCmpt(world.colors, vb.id)->value;

		if (!cmpt::getCmpt(world.scales, vb.id))
			scale = glm::vec3(1, 1, 1);
		else
			scale = cmpt::getCmpt(world.scales, vb.id)->value;

		glm::mat4 model;
		model = glm::translate(model, pos);
		model *= glm::toMat4(orientation);
		model = glm::scale(model, scale);

		shader.setMat4("model", model);
		shader.setVec4("color", color);

		glBindVertexArray(vb.value.id);
		glDrawArrays(vb.value.mode, 0, vb.value.count);
	}

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