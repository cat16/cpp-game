#include "renderer.hpp"

#include <glew.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtc/type_ptr.hpp>

#include <iostream>

#include "util.hpp"
#include "component.hpp"

//Camera class

Camera::Camera(glm::vec3 pos, int width, int height) : pos(pos), width(width), height(height) {}

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

	glClearColor(0.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader.getID());

	glm::mat4 view;
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)camera.width / (float)camera.height, 0.1f, 100.0f);
	view = glm::lookAt(camera.pos, camera.pos + camera.getForward(), camera.getUp());

	shader.setMat4("projection", projection); // might be moved outside the render function
	shader.setMat4("view", view);

	for (cmpt::vertexBuffer vb : world.vertexBuffers) {

		glm::vec3 pos = cmpt::getCmpt(world.positions, vb.id)[0].value;
		glm::vec4 color = cmpt::getCmpt(world.colors, vb.id)[0].value;

		glBindVertexArray(vb.value);

		glm::mat4 model;

		model = glm::translate(model, pos);
		//model *= camera.getQuat()[0][0];
		shader.setMat4("model", model);

		shader.setVec4("color", color);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	SDL_GL_SwapWindow(window);
}

void Renderer::init(const char * vertex_file_path, const char * fragment_file_path) {
	shader.init(vertex_file_path, fragment_file_path);

	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_LINE_SMOOTH);
}