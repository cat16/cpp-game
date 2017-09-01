/*
* Util.h
*
*  Created on: Jul 25, 2017
*      Author: Bryan McShea
*/

#pragma once


#include <glew.h>
#include <glm.hpp>
#include <gtx/quaternion.hpp>
#include <btBulletDynamicsCommon.h>

#include <string>
#include <iostream>
#include <fstream>

namespace util {

	enum rotationEnum {
		ANGLE = 0, XAXIS = 1, YAXIS = 2, ZAXIS = 3
	};

	const static long double PI = 3.141592653589793238L;
	const static long double TAU = PI * 2;
	const static long double G = 0.000000000066742;

	glm::vec4 toRadians(glm::vec4 rotation);

	float toRadiansf(float rotation);

	double toDegrees(double rotation);

	void rotate(glm::quat rotation);
	void unRotate(glm::quat rotation);

	glm::vec3 move3d(glm::vec3 p1, glm::vec3 p2, GLfloat speed);
	GLfloat distance3f(glm::vec3 p1, glm::vec3 p2);

	btVector3 btv3(glm::vec3 vec);
	glm::vec3 glmv3(const btVector3 &vec);

	std::string loadFile(const char * path);
}