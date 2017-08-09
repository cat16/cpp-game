#pragma once
/*
* entity.hpp
*
*  Created on: Jul 31, 2017
*      Author: Bryan McShea
*/

#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtx/quaternion.hpp"
#include "btBulletDynamicsCommon.h"

namespace entity {
	class Object {
	public:
		bool gravity = true;

		btRigidBody * body;

		virtual void draw() = 0;
		glm::vec3 getPos();
		glm::quat getQuat();

		void setPos(glm::vec3 pos);

		virtual Object * clone() = 0;
	};

	class Colorable {
	public:
		Colorable(glm::vec4 color);
		glm::vec4 color;
	};

	class Voxel : public Object, public Colorable {
	public:
		Voxel(const btVector3 &pos, const btQuaternion &rotation, glm::vec4 color);

		void draw();

		Voxel * clone();
	};

	class Sphere : public Object, public Colorable {
	public:
		Sphere(const btVector3 &pos, btScalar radius, btScalar mass, glm::vec4 color);

		void draw();
		btScalar getRadius();

		Sphere * clone();
	};

	class Planet : public Sphere {
	public:
		long int mass;

		Planet(const btVector3 &pos, btScalar radius, btScalar mass, glm::vec4 color);

	};

	class Star : public Sphere {
	public:
		Star(const btVector3 &pos, btScalar radius, glm::vec4 color);
		void draw();
	};
}