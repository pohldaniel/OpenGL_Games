#pragma once
#include <vector>
#include <algorithm>
#include <glm/glm.hpp>
#include <glm/gtx/transform.hpp>
#include <glm/gtc/type_ptr.hpp>
#include <glm/gtx/rotate_vector.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <engine/Shader.h>

#include "SimulationObject.h"
#include "VehiclePhysics.h"

class VehicleObject : public SimulationObject{

public:

	VehicleObject() = default;
	void draw() override;

	const btTransform getWorldTransform() const;
	const btVector3& getLinearVelocity() const;

	std::string wheelObjPath;

	VehiclePhysics vehicle;

	int wheelMatrixLength;
	std::vector<glm::mat4> wheelMatrices;
	void UpdateModelMatrix();

	ObjModelNew* m_wheel;
	Shader* shader;

	btVector3 vehiclePosition;
};