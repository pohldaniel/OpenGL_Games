#pragma once

#include <glm/gtc/quaternion.hpp>
#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>
#include <engine/Material.h>
#include <Scene/MeshNode.h>
#include "Rigidbody.h"

class Entity : public MeshNode {

public:

	Entity(AssimpModel* model);
	virtual ~Entity() = default;

	virtual void draw(const Camera& camera);
	virtual void update(const float dt);
	virtual void fixedUpdate(float fdt);

	virtual const Material& getMaterial() const;

	void markForDelete();
	bool isMarkForDelete();

	bool m_disabled;
	bool m_isStatic;
	bool m_markForDelete;

	Rigidbody rigidbody;
};