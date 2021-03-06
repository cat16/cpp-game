/*
* World.h
*
*  Created on: Jul 25, 2017
*      Author: Bryan McShea
*/

#pragma once

#include <glew.h>
#include "glm.hpp"
#include "gtc/matrix_transform.hpp"
#include "gtx/quaternion.hpp"
#include "btBulletDynamicsCommon.h"

#include <vector>
#include <map>

#include "component.hpp"

class World {
private:
	btBroadphaseInterface* broadphase;
	btDefaultCollisionConfiguration* collisionConfiguration;
	btCollisionDispatcher* dispatcher;
	btSequentialImpulseConstraintSolver* solver;
	btDiscreteDynamicsWorld* dynamicsWorld;

	std::vector<cmpt::rigidBody> rigidBodies;
public:
	std::map<std::string, vbd::vao> vbs;

	//components

	std::vector<cmpt::pos> positions;
	std::vector<cmpt::orientation> orientations;
	std::vector<cmpt::scale> scales;
	std::vector<std::vector<cmpt::vao>> layers;
	std::vector<cmpt::color> colors;
	std::vector<cmpt::material> materials;
	std::vector<cmpt::pointLight> pointLights;
	std::vector<cmpt::gravity> gravities;

	//component defaults

	struct Defaults {
		const glm::quat orientation = glm::quat(0, 0, 0, 1);
		const glm::vec3 scale = glm::vec3(1, 1, 1);
		const glm::vec4 color = glm::vec4(1, 1, 1, 0.5);
		const Shader::Material material = Shader::Material(glm::vec3(0.1, 0.1, 0.1), glm::vec3(1, 1, 1), glm::vec3(0.5, 0.5, 0.5), 32);
	} defaults;

	bool paused = false;

	World();

	void removeId(int id);

	//special component manipulation

	void addRigidBody(cmpt::rigidBody b);
	void removeRigidBody(int id);

	//easy component creation

	void createBox(int id, glm::vec3 pos, glm::quat orientation, glm::vec3 dimensions, glm::vec4 color, int layer);
	void createSphere(int id, glm::vec3 pos, glm::quat orientation, btScalar radius, glm::vec4 color, int layer);

	void createRigidBox(int id, glm::vec3 pos, glm::quat orientation, glm::vec3 dimensions, glm::vec4 color, int layer, btScalar mass);
	void createRigidSphere(int id, glm::vec3 pos, glm::quat orientation, btScalar radius, glm::vec4 color, int layer, btScalar mass);



	void updateDrag();
	void updateGravity();
	void updateRigidBodies();

	void update(double delta);
	void cleanUp();

	btDiscreteDynamicsWorld* getDynamicsWorld();

	cmpt::rigidBody* raycast(glm::vec3 start, glm::vec3 end);
};