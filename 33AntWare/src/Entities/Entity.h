#pragma once

#include <glm/glm.hpp>
#include <glm/gtc/quaternion.hpp>
#include <engine/Camera.h>
#include <engine/ObjModel.h>
#include <engine/AssimpModel.h>

#include <Objects/SceneNode.h>

#include "Rigidbody.h"
#include "MaterialAW.h"
#include "Mesh.h"
#include "AABB.h"
#include "TransformAW.h"

class Entity : public SceneNode {

public:
	Entity();
	Entity(std::shared_ptr<aw::Mesh> mesh, aw::Material material);
	//Entity(Entity const& rhs);
	//Entity& operator=(const Entity& rhs);


	virtual void draw(const Camera& camera);
	virtual void update(const float dt);
	virtual void fixedUpdate(float fdt);

	void constructAABB();
	void recalculateAABB();

	bool m_disabled;
	bool m_isStatic;

	aw::Rigidbody rigidbody;
	std::shared_ptr<aw::Mesh> meshPtr;
	aw::Material material;
	aw::AABB aabb;
	aw::Transform transform;
};