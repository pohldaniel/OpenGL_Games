#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <engine/Texture.h>
#include <engine/Camera.h>
#include <engine/AssimpModel.h>

#include <Scene/MeshNode.h>

#include "Rigidbody.h"
#include "MaterialAW.h"

#include "AABB.h"

class Entity : public MeshNode {

public:

	Entity(std::shared_ptr<aw::Mesh> mesh, aw::Material material);
	Entity(AssimpModel* model, aw::Material material);
	Entity(std::shared_ptr<aw::Mesh> mesh, AssimpModel* model, aw::Material material);

	virtual void draw(const Camera& camera);
	virtual void update(const float dt);
	virtual void fixedUpdate(float fdt);

	void constructAABB();
	void recalculateAABB();

	bool m_disabled;
	bool m_isStatic;

	aw::Rigidbody rigidbody;
	
	aw::Material material;
	aw::AABB aabb;

	//const Texture& m_texture;
};