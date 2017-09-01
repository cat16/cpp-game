#pragma once

#include <glew.h>
#include <btBulletDynamicsCommon.h>
#include <glm.hpp>
#include <gtc/matrix_transform.hpp>
#include <gtx/quaternion.hpp>

#include <vector>
#include <algorithm>

#include "VAO.hpp"
#include "shader.hpp"

namespace cmpt {

	//base component

	struct component {
		int id;
		component(int id);
	};

	//====================[ derived components ]====================

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

	struct scale : public component {
		bool operator==(const scale& other) { return value == other.value; };
		glm::vec3 value;
		scale(int id, glm::vec3 value);
	};

	struct color : public component {
		bool operator==(const color& other) { return value == other.value; };
		glm::vec4 value;
		color(int id, glm::vec4 value);
	};

	struct material : public component {
		bool operator==(const material& other) { return value == other.value; };
		Shader::Material value;
		material(int id, Shader::Material material);
	};

	struct pointLight : public component {
		bool operator==(const pointLight& other) { return value == other.value; };
		Shader::PointLight value;
		pointLight(int id, Shader::PointLight light);
	};

	struct vao : public component {
		bool operator==(const vao& other) { return value.id == other.value.id; };
		vbd::vao value;
		vao(int id, vbd::vao value);
	};

	struct rigidBody : public component {
		bool operator==(const rigidBody& other) { return value == other.value; };
		btRigidBody * value;

		rigidBody(int id, btRigidBody * value);
		rigidBody(int id, glm::vec3 pos, glm::quat orientation, btCollisionShape *shape, btScalar mass);

		glm::vec3 getPos();
		glm::quat getQuat();
		void setPos(glm::vec3 pos);
	};

	struct gravity : public component {
		bool operator==(const gravity& other) { return value == other.value; };
		btScalar value;
		gravity(int id, btScalar value);
	};

	//====================[ utility functions for components ]====================

	template<class T>
	void insert_sorted(std::vector<T> & vec, T const& c) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");

		vec.insert(
			std::upper_bound(
				vec.begin(), vec.end(), c,
				[](const component &c1, const component &c2) { return c1.id < c2.id; }
			), c
		);
	}

	template<typename T>
	void updateCmpt(std::vector<T> &vec, T c) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");

		if (getCmpt(vec, c.id)) {
			getCmpt(vec, c.id)->value = c.value;
		}
		else {
			addCmpt(vec, c);
		}
	}

	template<typename T>
	T* getCmpt(std::vector<T> &vec, int id) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");

		for (unsigned int i = 0; i < vec.size(); i++) {
			if (vec[i].id == id)
				return &vec[i];
		}

		return NULL;
	}

	template<typename T>
	T* getCmptSafe(std::vector<T> &vec, T* def) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");

		for (unsigned int i = 0; i < vec.size(); i++) {
			if (vec[i].id == def->id)
				return &vec[i];
		}

		return def;
	}

	template<typename T>
	void addCmpt(std::vector<T> &vec, T c) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");

		insert_sorted(vec, c);
	}

	template<typename T>
	void removeCmpt(std::vector<T> &vec, int id) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");

		vec.erase(std::remove_if(vec.begin(), vec.end(),
			[id](const component &c) { return c.id == id; }), vec.end());
	}

	template<typename T>
	void copyCmpt(std::vector<T> &vec, int oldId, int newId) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");
		if (getCmpt(vec, oldId))
			addCmpt(vec, { newId, getCmpt(vec, oldId)->value });
	}

	template<typename T>
	void copyCmpt(std::vector<T> &vecFrom, std::vector<T> &vecTo, int oldId, int newId) {
		static_assert(std::is_base_of<component, T>::value, "T must derive from component");
		if (getCmpt(vecFrom, oldId))
			addCmpt(vecTo, { newId, getCmpt(vecFrom, oldId)->value });
	}

}