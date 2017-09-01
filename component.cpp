#include "component.hpp"

cmpt::component::component(int id) : id(id) {}

cmpt::pos::pos(int id, glm::vec3 value) : component(id), value(value) {}
cmpt::orientation::orientation(int id, glm::quat value) : component(id), value(value) {}
cmpt::scale::scale(int id, glm::vec3 value) : component(id), value(value) {}
cmpt::color::color(int id, glm::vec4 value) : component(id), value(value) {}
cmpt::vao::vao(int id, vbd::vao value) : component(id), value(value) {}
cmpt::rigidBody::rigidBody(int id, btRigidBody * value) : component(id), value(value) {}
cmpt::gravity::gravity(int id, btScalar value) : component(id), value(value) {}
cmpt::material::material(int id, Shader::Material material) : component(id), value(material) {}
cmpt::pointLight::pointLight(int id, Shader::PointLight light) : component(id), value(light) {}

cmpt::rigidBody::rigidBody(int id, glm::vec3 pos, glm::quat orientation, btCollisionShape *shape, btScalar mass) : component(id) {

	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform({orientation.x, orientation.y, orientation.z, -orientation.w}, util::btv3(pos)));
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);

	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
	value = new btRigidBody(rigidBodyCI);
	value->setSleepingThresholds(0.01, 0.01);
}



glm::vec3 cmpt::rigidBody::getPos() {
	btTransform trans;
	value->getMotionState()->getWorldTransform(trans);
	return glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
}

glm::quat cmpt::rigidBody::getQuat() {
	return glm::quat(value->getOrientation().getW(), value->getOrientation().getX(), value->getOrientation().getY(), value->getOrientation().getZ());
}

void cmpt::rigidBody::setPos(glm::vec3 pos) {
	btTransform transform;
	transform.setOrigin(util::btv3(pos));
	transform.setRotation(value->getOrientation());
	value->setWorldTransform(transform);
	value->getMotionState()->setWorldTransform(transform);
	value->setLinearVelocity(btVector3(0, 0, 0));
}