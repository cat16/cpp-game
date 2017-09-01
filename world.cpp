/*
* World.cpp
*
*  Created on: Jul 27, 2017
*      Author: cat16
*/

#include "world.hpp"

#include <vector>
#include <algorithm>

#include "util.hpp"
#include "VAO.hpp"


World::World() {
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);
	dynamicsWorld->setGravity(btVector3(0, 0, 0));
}

void World::addRigidBody(cmpt::rigidBody b) {
	cmpt::addCmpt(rigidBodies, b);
	dynamicsWorld->addRigidBody(b.value);
}

void World::removeRigidBody(int id) {
	if (cmpt::getCmpt(rigidBodies, id)) {
		cmpt::removeCmpt(rigidBodies, id);
		dynamicsWorld->removeRigidBody(cmpt::getCmpt(rigidBodies, id)->value);
	}
}

void World::removeId(int id) {
	cmpt::removeCmpt(positions, id);
	cmpt::removeCmpt(orientations, id);
	cmpt::removeCmpt(scales, id);
	for (auto layer : layers) {
		cmpt::removeCmpt(layer, id);
	}
	cmpt::removeCmpt(colors, id);
	cmpt::removeCmpt(materials, id);
	cmpt::removeCmpt(rigidBodies, id);
	cmpt::removeCmpt(gravities, id);
}

//easy component creation

void World::createBox(int id, glm::vec3 pos, glm::quat orientation, glm::vec3 dimensions, glm::vec4 color, int layer) {
	cmpt::addCmpt(positions, { id, pos });
	cmpt::addCmpt(orientations, { id, orientation });
	cmpt::addCmpt(scales, { id, dimensions });
	cmpt::addCmpt(colors, { id, color });
	cmpt::addCmpt(layers[layer], { id, vbs["cube"] });
}

void World::createSphere(int id, glm::vec3 pos, glm::quat orientation, btScalar radius, glm::vec4 color, int layer) {
	cmpt::addCmpt(positions, { id, pos });
	cmpt::addCmpt(orientations, { id, orientation });
	cmpt::addCmpt(scales, { id, glm::vec3(radius, radius, radius) });
	cmpt::addCmpt(colors, { id, color });
	cmpt::addCmpt(layers[layer], { id, vbs["sphere"] });
}

void World::createRigidBox(int id, glm::vec3 pos, glm::quat orientation, glm::vec3 dimensions, glm::vec4 color, int layer, btScalar mass) {
	createBox(id, pos, orientation, dimensions, color, layer);
	addRigidBody({ id, pos, orientation, new btBoxShape({dimensions.x / 2, dimensions.y / 2, dimensions.z / 2}), mass });
}

void World::createRigidSphere(int id, glm::vec3 pos, glm::quat orientation, btScalar radius, glm::vec4 color, int layer, btScalar mass) {
	createSphere(id, pos, orientation, radius, color, layer);
	addRigidBody({ id, pos, orientation, new btSphereShape(radius), mass });
}

void World::updateDrag() {
	for (cmpt::rigidBody rb : rigidBodies) {
		rb.value->applyDamping(10);
	}
}

void World::updateGravity() {
	//gravity loop
	auto gPosItr = positions.begin();
	for (auto gItr = gravities.begin(); gItr != gravities.end(); ++gItr) {
		gPosItr = std::find_if(gPosItr, positions.end(), [gItr](const cmpt::component &c) { return c.id >= gItr->id; });

		if (gPosItr == positions.end()) {
			break;
		}
		if (gPosItr->id > gItr->id) {
			continue;
		} else {
			//rigidbody loop
			auto rPosItr = positions.begin();
			for (auto rItr = rigidBodies.begin(); rItr != rigidBodies.end(); ++rItr) {
				rPosItr = std::find_if(rPosItr, positions.end(), [rItr](const cmpt::component &c) { return c.id >= rItr->id; });

				if (rPosItr == positions.end()) {
					break;
				}
				if (rPosItr->id > rItr->id) {
					continue;
				} else {
					//apply gravity
					if (rItr->value->isActive()) {
						btScalar d = util::distance3f(gPosItr->value, rPosItr->value);
						if (d < 1000) {
							btScalar force = (util::G * glm::pow(10, 12)) * (gItr->value * rItr->value->getInvMass()) / glm::pow(d, 2);
							glm::vec3 gravity = util::move3d(rPosItr->value, gPosItr->value, -force);
							rItr->value->applyForce(btVector3(gravity.x, gravity.y, gravity.z), btVector3(0, 0, 0));
						}
					}
				}
			}
		}
	}
}

void World::updateRigidBodies() {
	auto posItr = positions.begin();
	auto orientationItr = orientations.begin();

	for (auto rbItr = rigidBodies.begin(); rbItr != rigidBodies.end(); ++rbItr) {

		auto nextId = [rbItr](const cmpt::component &c) { return c.id >= rbItr->id; };

		posItr = std::find_if(posItr, positions.end(), nextId);
		orientationItr = std::find_if(orientationItr, orientations.end(), nextId);

		if (rbItr->id == posItr->id)
			posItr->value = rbItr->getPos();
		if (rbItr->id == orientationItr->id)
			orientationItr->value = rbItr->getQuat();

	}
}

void World::update(double delta) {
	if (!paused) {
		updateDrag();
		updateGravity();
		dynamicsWorld->stepSimulation(1 / 60.f, 10 * delta);
		updateRigidBodies();
	}
}

void World::cleanUp() {
	delete dynamicsWorld;
	delete solver;
	delete dispatcher;
	delete collisionConfiguration;
	delete broadphase;
}

btDiscreteDynamicsWorld* World::getDynamicsWorld() {
	return dynamicsWorld;
}

cmpt::rigidBody* World::raycast(glm::vec3 s, glm::vec3 e) {

	btVector3 start = util::btv3(s);
	btVector3 end = util::btv3(e);

	btCollisionWorld::ClosestRayResultCallback result(start, end);

	dynamicsWorld->rayTest(start, end, result);

	if (result.hasHit()) {
		if (const btRigidBody* rb = dynamic_cast<const btRigidBody *>(result.m_collisionObject)) {
			for (unsigned int i = 0; i < rigidBodies.size(); i++) {
				if (rb == rigidBodies[i].value) {
					return &rigidBodies[i];
					break;
				}
			}
		}
	}

	return NULL;
}