#pragma once

#include <Physics/Physics.h>
#include "SceneNode.h"

class CollisionNode : public SceneNode {

public:

	CollisionNode(btCollisionObject* collisionObject);
	~CollisionNode();

	const Matrix4f& getWorldTransformation() const override;

	using SceneNode::addChild;
	void addChild(CollisionNode* node);
	btCollisionObject* getCollisionObject() const;
	void setActive(bool active);
	bool isActive();

protected:

	btCollisionObject* m_collisionObject;
	bool m_isActive;
};