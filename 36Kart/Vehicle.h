#pragma once
#include <glm/glm.hpp>
#include <glm/gtx/vector_angle.hpp>
#include <glm/gtc/type_ptr.hpp>

#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/Material.h>
#include <engine/scene/SequenceNode.h>

#include "VehiclePhysics.h"

class Vehicle : public SequenceNode {

public:

	Vehicle(const MeshSequence& meshSequence);
	virtual ~Vehicle();

	virtual void draw();
	virtual void update(const float dt);
	virtual const Material& getMaterial() const;

	const btTransform& getWorldTransform() const;
	const btVector3& getLinearVelocity() const;
	void roate(float x, float y, float z);

	short getMaterialIndex() const;
	void setMaterialIndex(short index) const;
	short getTextureIndex() const;
	void setTextureIndex(short index) const;

	VehiclePhysics vehicle;

protected:

	mutable short m_materialIndex;
	mutable short m_textureIndex;

private:

	void updateModelMatrix();

	std::vector<glm::mat4> wheelMatrices;
	glm::mat4 objModelMatrix;
};