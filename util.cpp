/*
* Util.cpp
*
*  Created on: Jul 27, 2017
*      Author: cat16
*/

#include "util.hpp"

glm::vec3 util::move3d(glm::vec3 p1, glm::vec3 p2, GLfloat speed) {
	GLfloat xd = p1.x - p2.x;
	GLfloat yd = p1.y - p2.y;
	GLfloat zd = p1.z - p2.z;

	GLfloat d = sqrt(xd * xd + yd * yd + zd * zd);

	return glm::vec3(xd / d * speed, yd / d * speed, zd / d * speed);
}

GLfloat util::distance3f(glm::vec3 p1, glm::vec3 p2) {
	GLfloat xd = p1.x - p2.x;
	GLfloat yd = p1.y - p2.y;
	GLfloat zd = p1.z - p2.z;

	GLfloat d = sqrt(xd * xd + yd * yd + zd * zd);

	return d;
}

glm::vec4 util::toRadians(glm::vec4 rotation) {
	glm::vec4 rotation2 = glm::vec4(rotation);
	rotation2[util::ANGLE] = 360 * rotation[ANGLE] * util::TAU / util::PI;
	return rotation2;
}

float util::toRadiansf(float rotation) {
	return rotation * TAU;
}

double util::toDegrees(double rotation) {
	return rotation * 360;
}

void util::rotate(glm::quat rotation) {
	glRotatef(rotation.w * 360, rotation.x * 360 / TAU, rotation.y * 360 / TAU, rotation.z * 360 / TAU);
}

void util::unRotate(glm::quat rotation) {
	glm::quat rotation2 = glm::quat(rotation);
	rotation2.w *= -1;
	rotate(rotation2);
}

btVector3 util::btv3(glm::vec3 vec) {
	return btVector3(vec.x, vec.y, vec.z);
}

glm::vec3 util::glmv3(const btVector3 &vec) {
	return glm::vec3(vec.getX(), vec.getY(), vec.getZ());
}