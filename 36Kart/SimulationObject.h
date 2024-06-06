#pragma once

#include<string>
#include<glm/glm.hpp>
#include<glm/gtc/quaternion.hpp>

#include "ObjModelNew.h"

class SimulationObject {

public:

	virtual void draw();

	std::string modelPath;
	std::string texPath;

	glm::vec3 position;
	glm::quat rotation;

	glm::mat4 objModelMatrix; //Object's Model Matrix

	enum SimType {
		Instance,
		Character,
		Vehicle,
		Projectile,
	};

	SimType type;

	SimulationObject() = default;

	// virtual void setPosition(const glm::vec3& pos);

	const glm::vec3& getPosition() const {
		return position;
	}

	const glm::quat& getRotation() const {
		return rotation;
	}
	ObjModelNew* m_model;
	// virtual void setRotation(const glm::quat& orientation);
};