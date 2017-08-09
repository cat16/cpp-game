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

	std::vector<cmpt::pos> positions;
	std::vector<cmpt::orientation> orientations;
	std::vector<cmpt::vertexBuffer> vertexBuffers;
	std::vector<cmpt::colorBuffer> colorBuffers;
	std::vector<cmpt::gravity> gravities;

	bool paused = false;

	World();

	void addRigidBody(cmpt::rigidBody b);
	void removeRigidBody(cmpt::rigidBody b);
	void removeRigidBody(int id);

	bool idExists(int id);
	void removeId(int id);

	void createBox   (int id, glm::vec3 pos, glm::quat orientation, glm::vec4 color, glm::vec3 dimensions);
	void createSphere(int id, glm::vec3 pos, glm::quat orientation, glm::vec4 color, btScalar r);

	void update();
	void generate();
	void cleanUp();

	btDiscreteDynamicsWorld* getDynamicsWorld();

	cmpt::rigidBody raycast(glm::vec3 start, glm::vec3 end);
};