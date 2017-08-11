//unused - still kept for method code

/*#include "entity.hpp"

#include <glew.h>

#include "util.hpp"

//Object class

glm::vec3 entity::Object::getPos() {
	btTransform trans;
	body->getMotionState()->getWorldTransform(trans);
	return glm::vec3(trans.getOrigin().getX(), trans.getOrigin().getY(), trans.getOrigin().getZ());
}

glm::quat entity::Object::getQuat() {
	return glm::quat(body->getOrientation().getX(), body->getOrientation().getY(), body->getOrientation().getZ(), body->getOrientation().getW());
}

void entity::Object::setPos(glm::vec3 pos) {
	btTransform transform;
	transform.setOrigin(util::btv3(pos));
	transform.setRotation(body->getOrientation());
	body->setWorldTransform(transform);
	body->getMotionState()->setWorldTransform(transform);
	body->setLinearVelocity(btVector3(0, 0, 0));
}

//junk

entity::Colorable::Colorable(glm::vec4 c){
	color = c;
}

//Voxel class

entity::Voxel::Voxel(const btVector3 &pos, const btQuaternion &rotation, glm::vec4 color) : Colorable(color) {
	btScalar mass = 1;
	btScalar size = 1;
	btBoxShape* shape = new btBoxShape(btVector3(size / 2, size / 2, size / 2));
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(rotation, pos));
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
	body = new btRigidBody(rigidBodyCI);
}

void entity::Voxel::draw() {

	glBegin(GL_QUADS);

	GLfloat size = 1;

	GLfloat vert = (size / 2);
	GLfloat iVert = (-size / 2);

	// front face
	glColor4f(color.x, color.y, color.z, color.w);
	glNormal3f(0, 0, 1);
	glVertex3f(vert, vert, vert);
	glVertex3f(iVert, vert, vert);
	glVertex3f(iVert, iVert, vert);
	glVertex3f(vert, iVert, vert);
	// left face
	glNormal3f(-1, 0, 0);
	glVertex3f(iVert, vert, vert);
	glVertex3f(iVert, iVert, vert);
	glVertex3f(iVert, iVert, iVert);
	glVertex3f(iVert, vert, iVert);
	// back face
	glNormal3f(0, 0, -1);
	glVertex3f(vert, vert, iVert);
	glVertex3f(iVert, vert, iVert);
	glVertex3f(iVert, iVert, iVert);
	glVertex3f(vert, iVert, iVert);
	// right face
	glNormal3f(1, 0, 0);
	glVertex3f(vert, vert, vert);
	glVertex3f(vert, iVert, vert);
	glVertex3f(vert, iVert, iVert);
	glVertex3f(vert, vert, iVert);
	// top face
	glNormal3f(0, 1, 0);
	glVertex3f(vert, vert, vert);
	glVertex3f(iVert, vert, vert);
	glVertex3f(iVert, vert, iVert);
	glVertex3f(vert, vert, iVert);
	// bottom face
	glNormal3f(0, -1, 0);
	glVertex3f(vert, iVert, vert);
	glVertex3f(iVert, iVert, vert);
	glVertex3f(iVert, iVert, iVert);
	glVertex3f(vert, iVert, iVert);
	glEnd();
}

entity::Voxel * entity::Voxel::clone() {
	return new Voxel(*this);
}



entity::Sphere::Sphere(const btVector3 &pos, btScalar radius, btScalar mass, glm::vec4 color) : Colorable(color) {
	mass = 0;
	btSphereShape* shape = new btSphereShape(radius);
	btDefaultMotionState* motionState = new btDefaultMotionState(btTransform(btQuaternion(0, 0, 0, 1), pos));
	btVector3 inertia(0, 0, 0);
	shape->calculateLocalInertia(mass, inertia);
	btRigidBody::btRigidBodyConstructionInfo rigidBodyCI(mass, motionState, shape, inertia);
	body = new btRigidBody(rigidBodyCI);
}

void entity::Sphere::draw() {
	glColor3f(color.x, color.y, color.z);
	GLUquadricObj *quadric;
	quadric = gluNewQuadric();
	gluSphere(quadric, getRadius(), getRadius() * 10, getRadius() * 10);
}

btScalar entity::Sphere::getRadius() {
	return ((btSphereShape*)body->getCollisionShape())->getRadius();
}

entity::Sphere * entity::Sphere::clone() {
	return new Sphere(*this);
}



entity::Planet::Planet(const btVector3 &pos, btScalar radius, btScalar mass, glm::vec4 color) : Sphere(pos, radius, 0, color), mass(mass) {}



entity::Star::Star(const btVector3 &pos, btScalar radius, glm::vec4 color) : Sphere(pos, radius, 0, color) {
	body->setActivationState(DISABLE_SIMULATION);
}

void entity::Star::draw() {
	GLfloat position[] = { 0, 0, 0, 1.0f };
	glLightfv(GL_LIGHT0, GL_POSITION, position);
	Sphere::draw();
}*/