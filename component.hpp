#pragma once

#include <glew.h>
#include <btBulletDynamicsCommon.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>

#include <vector>
#include <algorithm>

namespace cmpt {

	struct component {
		int id;
		component(int id);
	};

	struct pos : public component {
		bool operator==(const pos& other) { return value == other.value; };
		glm::vec3 value;
		pos(int id, glm::vec3 value);
	};

	struct orientation : public component {
		bool operator==(const orientation& other) { return value == other.value; };
		glm::quat value;
		orientation(int id, glm::quat value);
	};

	struct color : public component {
		bool operator==(const color& other) { return value == other.value; };
		glm::vec4 value;
		color(int id, glm::vec4 value);
	};

	struct vertexBuffer : public component {
		bool operator==(const vertexBuffer& other) { return value == other.value; };
		GLuint value;
		vertexBuffer(int id, GLuint value);
	};

	struct rigidBody : public component {
		bool operator==(const rigidBody& other) { return value == other.value; };
		btRigidBody * value;
		rigidBody(int id, btRigidBody * value);
		glm::vec3 getPos();
		glm::quat getQuat();
	};

	struct gravity : public component {
		bool operator==(const gravity& other) { return value == other.value; };
		btScalar value;
		gravity(int id, btScalar value);
	};

	template<typename T>
	std::vector<T> getCmpt(std::vector<T> &arr, int id) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");

		std::vector<T> matching;
		std::copy_if(arr.begin(), arr.end(), std::back_inserter(matching), [id](const component &c) {return c.id == id; });
		return matching;
	}

	template<typename T>
	void removeCmpt(std::vector<T> &arr, int id) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");

		arr.erase(std::remove_if(arr.begin(), arr.end(),
			[id](const component &c) { return c.id == id; }), arr.end());
	}

}