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
glm::vec3 Camera::getRight() { return conjugate(orientation) * glm::vec3(1.0f, 0.0f, 0.0f); }

glm::quat Camera::getQuat() {
	return orientation;
}

//Renderer class

Renderer::Renderer(Shader s) : shader(s) {

}

void Renderer::setWindow(SDL_Window * w) {
	window = w;
}

void Renderer::render(World world, Camera camera) {

	glClearColor(0.f, 1.f, 1.f, 1.0f);
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	glUseProgram(shader.getID());

	
	glm::mat4 view;
	glm::mat4 projection;
	projection = glm::perspective(glm::radians(45.0f), (float)camera.width / (float)camera.height, 0.1f, 100.0f);
	view = glm::lookAt(glm::vec3(camera.pos.x, camera.pos.y, camera.pos.z), glm::vec3(0.0, 0.0, 0.0), glm::vec3(0.0, 1.0, 0.0));
	
	shader.setMat4("projection", projection); // might be moved outside the render function
	shader.setMat4("view", view);

	for (cmpt::vertexBuffer vb : world.vertexBuffers) {

		glBindVertexArray(vb.value);

		glm::mat4 model;

		glm::vec3 pos = cmpt::getCmpt(world.positions, vb.id)[0].value;
		model = glm::translate(model, pos);
		//model *= camera.getQuat()[0][0];
		shader.setMat4("model", model);

		glDrawArrays(GL_TRIANGLES, 0, 36);
	}

	/*glMatrixMode(GL_PROJECTION);
	glPushMatrix();
	glLoadIdentity();
	glMatrixMode(GL_MODELVIEW);
	glLoadIdentity();
	glDisable(GL_CULL_FACE);
	glClear(GL_DEPTH_BUFFER_BIT);

	if (gui)
		gui->draw(world);*/

	SDL_GL_SwapWindow(window);
}

void Renderer::init() {
	glEnable(GL_DEPTH_TEST);
	glDepthFunc(GL_LESS);

	glEnable(GL_LINE_SMOOTH);
}