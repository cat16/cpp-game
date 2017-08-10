#include "component.hpp"

cmpt::component::component(int id) : id(id) {}

cmpt::pos::pos(int id, glm::vec3 value) : component(id), value(value){}
cmpt::orientation::orientation(int id, glm::quat value) : component(id), value(value) {}
cmpt::color::color(int id, glm::vec4 value) : component(id), value(value) {}
cmpt::vertexBuffer::vertexBuffer(int id, GLuint value) : component(id), value(value) {}
cmpt::rigidBody::rigidBody(int id, btRigidBody * value) : component(id), value(value) {}
cmpt::gravity::gravity(int id, btScalar value) : component(id), value(value) {}



glm::vec3 cmpt::rigidBody::getPos() {
	btTransform trans;
	value->getMotionState()->getWorldTransform(trans);
	return glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
}

glm::quat cmpt::rigidBody::getQuat() {
	return glm::quat(value->getOrientation().getX(), value->getOrientation().getY(), value->getOrientation().getZ(), value->getOrientation().getW());
}