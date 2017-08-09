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
#include "VBD.hpp"


World::World() {
	broadphase = new btDbvtBroadphase();
	collisionConfiguration = new btDefaultCollisionConfiguration();
	dispatcher = new btCollisionDispatcher(collisionConfiguration);
	solver = new btSequentialImpulseConstraintSolver;
	dynamicsWorld = new btDiscreteDynamicsWorld(dispatcher, broadphase, solver, collisionConfiguration);

	dynamicsWorld->setGravity(btVector3(0, 0, 0));
	//btCollisionShape* groundShape = new btStaticPlaneShape(btVector3(0, 1, 0), 1);
	//btDefaultMotionState* groundMotionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), btVector3(0, 0, 0)));
	//btRigidBody::btRigidBodyConstructionInfo groundRigidBodyCI(0, groundMotionState, groundShape, btVector3(0, 0, 0));
	//btRigidBody* groundRigidBody = new btRigidBody(groundRigidBodyCI);
	//dynamicsWorld->addRigidBody(groundRigidBody);
}

void World::addRigidBody(cmpt::rigidBody b) {
	rigidBodies.push_back(b);
	dynamicsWorld->addRigidBody(b.value);
}

void World::removeRigidBody(cmpt::rigidBody b) {

	std::vector<cmpt::rigidBody>::iterator it = std::find(rigidBodies.begin(), rigidBodies.end(), b);
	if (it != rigidBodies.end())
		rigidBodies.erase(it);

	dynamicsWorld->removeRigidBody(b.value);
}

void World::removeRigidBody(int id) {

	std::vector<cmpt::rigidBody> bv = cmpt::getCmpt(rigidBodies, id);
	for (cmpt::rigidBody b : bv) {
		dynamicsWorld->removeRigidBody(b.value);
	}

	cmpt::removeCmpt(rigidBodies, bv[0].id);
}

bool World::idExists(int id) {
	for (cmpt::rigidBody c : rigidBodies) {
		if (c.id == id) return true;
	}

	for (cmpt::pos c : positions) {
		if (c.id == id) return true;
	}

	for (cmpt::orientation c : orientations) {
		if (c.id == id) return true;
	}

	for (cmpt::vertexBuffer c : vertexBuffers) {
		if (c.id == id) return true;
	}

	for (cmpt::colorBuffer c : colorBuffers) {
		if (c.id == id) return true;
	}

	for (cmpt::gravity c : gravities) {
		if (c.id == id) return true;
	}
}

void World::removeId(int id) {
	cmpt::removeCmpt(positions, id);
	cmpt::removeCmpt(orientations, id);
	cmpt::removeCmpt(vertexBuffers, id);
	cmpt::removeCmpt(colorBuffers, id);
	cmpt::removeCmpt(rigidBodies, id);
	cmpt::removeCmpt(gravities, id);
}

void World::createBox(int id, glm::vec3 pos, glm::quat orientation, glm::vec4 color, glm::vec3 dimensions) {
	positions.push_back({id, pos});
	orientations.push_back({id, orientation});
	//colorBuffers.push_back({ id, color });
	vertexBuffers.push_back({ id, vbd::cube});
}

void World::createSphere(int id, glm::vec3 pos, glm::quat orientation, glm::vec4 color, btScalar r) {
	positions.push_back({ id, pos });
	orientations.push_back({ id, orientation });
	//colorBuffers.push_back({ id, color });
	vertexBuffers.push_back({ id, vbd::sphere });
}

void World::update() {
	if (!paused) {
		for (cmpt::gravity g : gravities) {
			glm::vec3 gPos = cmpt::getCmpt(positions, g.id)[0].value;

			for (cmpt::rigidBody r : rigidBodies) {
				glm::vec3 rPos = cmpt::getCmpt(positions, r.id)[0].value;

				btScalar d = util::distance3f(gPos, rPos);
				if (d < 100) {
					btScalar force = (util::G * glm::pow(10, 12)) * (g.value * r.value->getInvMass()) / glm::pow(d, 2);
					glm::vec3 gravity = util::move3d(rPos, gPos, -force);
					r.value->applyForce(btVector3(gravity.x, gravity.y, gravity.z), btVector3(0, 0, 0));

					r.value->setActivationState(ACTIVE_TAG);
				}
			}
		}
		dynamicsWorld->stepSimulation(1 / 60.f, 10);
		for (cmpt::rigidBody r : rigidBodies) {
			cmpt::pos p = cmpt::getCmpt(positions, r.id)[0];
			p.value = r.getPos();
		}
	}
}

void World::generate() {

	createBox(1, glm::vec3(0, 0, 0), glm::quat(0, 0, 0, 1), glm::vec4(1, 0, 0, 1), glm::vec3(1, 1, 1));
	createBox(2, glm::vec3(0, 0, 0), glm::quat(0, 0, 0, 1), glm::vec4(1, 0, 0, 1), glm::vec3(1, 1, 1));

	//addObject(new entity::Planet(btVector3(0, -60, 0), 50, 100, glm::vec4(0.5, 0.5, 0.5, 1)));
	//addObject(new entity::Planet(btVector3(-40, -20, 0), 5, 5, glm::vec4(0.5, 0.5, 0.5, 1)));
	//addObject(new entity::Planet(btVector3(0, 50, 20), 30, 30, glm::vec4(0.5, 0.5, 0.5, 1)));

	//addObject(new entity::Star(btVector3(0, 15, 0), 2, glm::vec4(2, 2, 2, 0.5)));
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

cmpt::rigidBody World::raycast(glm::vec3 s, glm::vec3 e) {

	btVector3 start = util::btv3(s);
	btVector3 end = util::btv3(e);

	btCollisionWorld::ClosestRayResultCallback result(start, end);

	dynamicsWorld->rayTest(start, end, result);

	if (result.hasHit()) {
		if (const btRigidBody* rb = dynamic_cast<const btRigidBody *>(result.m_collisionObject)) {
			for (cmpt::rigidBody r : rigidBodies) {
				if (rb == r.value) {
					return r;
					break;
				}
			}
		}
	}

	return cmpt::rigidBody(NULL, NULL);
}