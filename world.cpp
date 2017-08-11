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
	/*std::vector<cmpt::rigidBody>::iterator it = std::find(rigidBodies.begin(), rigidBodies.end(), b);
	if (it != rigidBodies.end())
		rigidBodies.erase(it);

	dynamicsWorld->removeRigidBody(b.value);*/
}

void World::removeId(int id) {
	/*cmpt::removeCmpt(positions, id);
	cmpt::removeCmpt(orientations, id);
	cmpt::removeCmpt(vertexBuffers, id);
	cmpt::removeCmpt(colors, id);
	cmpt::removeCmpt(rigidBodies, id);
	cmpt::removeCmpt(gravities, id);*/
}

void World::createBox(int id, glm::vec3 pos, glm::quat orientation, glm::vec3 dimensions, glm::vec4 color) {
	positions.push_back({ id, pos });
	orientations.push_back({ id, orientation });
	scales.push_back({ id, dimensions });
	colors.push_back({ id, color });
	vertexBuffers.push_back({ id, vbs["cube"] });
}

void World::createRigidBox(int id, glm::vec3 pos, glm::quat orientation, glm::vec3 dimensions, glm::vec4 color) {
	createBox(id, pos, orientation, dimensions, color);

	btScalar mass = 1; //predefined atm; will be changed
	btBoxShape* shape = new btBoxShape(btVector3(dimensions.x / 2, dimensions.y / 2, dimensions.z / 2));
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(btVector3(orientation.x, orientation.y, orientation.z), orientation.w), util::btv3(pos)));
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
	addRigidBody({ id, new btRigidBody(rigidBodyCI) });
}

void World::createSphere(int id, glm::vec3 pos, glm::quat orientation, btScalar radius, glm::vec4 color) {
	positions.push_back({ id, pos });
	orientations.push_back({ id, orientation });
	scales.push_back({ id, glm::vec3(radius, radius, radius) });
	colors.push_back({ id, color });
	vertexBuffers.push_back({ id, vbs["sphere"] });
}

void World::createPlanet(int id, glm::vec3 pos, glm::quat orientation, btScalar radius, glm::vec4 color, btScalar gravity) {
	createSphere(id, pos, orientation, radius, color);
	gravities.push_back({ id, gravity });

	btScalar mass = 0; //predefined atm; will be changed
	btSphereShape* shape = new btSphereShape(radius);
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(btVector3(orientation.x, orientation.y, orientation.z), orientation.w), util::btv3(pos)));
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
	addRigidBody({ id, new btRigidBody(rigidBodyCI) });
}

void World::update() {
	if (!paused) {
		for (cmpt::gravity g : gravities) {
			glm::vec3 gPos;
			if (cmpt::getCmpt(positions, g.id))
				gPos = cmpt::getCmpt(positions, g.id)->value;
			else
				continue;


			for (cmpt::rigidBody r : rigidBodies) {
				if (!cmpt::getCmpt(gravities, r.id)) {
					glm::vec3 rPos;
					if (cmpt::getCmpt(positions, r.id))
						rPos = cmpt::getCmpt(positions, r.id)->value;
					else
						continue;

					btScalar d = util::distance3f(gPos, rPos);
					if (d < 1000) {
						btScalar force = (util::G * glm::pow(10, 12)) * (g.value * r.value->getInvMass()) / glm::pow(d, 2);
						glm::vec3 gravity = util::move3d(rPos, gPos, -force);
						r.value->applyForce(btVector3(gravity.x, gravity.y, gravity.z), btVector3(0, 0, 0));

						r.value->setActivationState(ACTIVE_TAG);
					}
				}
			}
		}
		dynamicsWorld->stepSimulation(1 / 60.f, 10);
		for (cmpt::rigidBody r : rigidBodies) {
			if (cmpt::getCmpt(positions, r.id)) {
				cmpt::pos * p = cmpt::getCmpt(positions, r.id);
				p->value = r.getPos();
			}

			if (cmpt::getCmpt(orientations, r.id)) {
				cmpt::orientation * o = cmpt::getCmpt(orientations, r.id);
				o->value = r.getQuat();
			}
		}
	}
}

void World::generate() {

	createRigidBox(1, glm::vec3(0, 0, -5), glm::quat(util::TAU/16, 0, 1, 0), glm::vec3(1, 1, 1), glm::vec4(1, 0, 0, 1));
	createBox     (2, glm::vec3(0, 0, -4), glm::quat(0, 0, 0, 1), glm::vec3(1, 1, 1), glm::vec4(0, 0, 1, 1));

	createPlanet(3, glm::vec3(0, 0, 5), glm::quat(0, 0, 0, 1), btScalar(2), glm::vec4(0, 1, 0, 1), btScalar(10));

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